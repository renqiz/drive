/*
  Copyright (c) 2018 Drive Foundation

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <assert.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <atomic>
#include "network/FileConnection.h"

namespace dfs
{
  namespace network
  {
    const char * FileConnection::ROOT = "transport";


    FileConnection::FileConnection(std::unique_ptr<IEndPoint> local, std::unique_ptr<IEndPoint> remote)
      : Connection(std::move(local), std::move(remote))
    {
      this->fepLocal = dynamic_cast<FileEndPoint *>(this->LocalEndPoint());
      this->fepRemote = dynamic_cast<FileEndPoint *>(this->RemoteEndPoint());
      assert(this->fepLocal);
      assert(this->fepRemote);
    }


    bool FileConnection::Send(const void * data, size_t len)
    {
      if (len == 0)
      {
        return true;
      }

      if (!data)
      {
        return false;
      }

      static std::atomic<uint32_t> packageId{0};

      // Path: ROOT/to/from/id

      Mkdir(this->fepLocal, this->fepRemote);

      char dirname[PATH_MAX];
      GetFolderPath(dirname, this->fepRemote, this->fepLocal);

      char filename[PATH_MAX];
      sprintf(filename, "%s/%08x", dirname, packageId++);

      bool completed = false;
      while (!completed)
      {
        int file = open(filename, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0755);
        if (file != -1)
        {
          if (flock(file, LOCK_EX | LOCK_NB) == 0)
          {
            write(file, &len, sizeof(len));
            write(file, data, len);
            completed = true;
          }

          close(file);
        }

        if (!completed)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
      }

      return true;
    }


    bool FileConnection::Receive(Buffer & buffer)
    {
      Mkdir(this->fepRemote, this->fepLocal);

      char dirname[PATH_MAX];
      GetFolderPath(dirname, this->fepRemote, this->fepLocal);

      bool success = false;

      while (!success)
      {
        uint32_t packageId = 0;
        DIR * dp = opendir(dirname);
        if (dp)
        {
          struct dirent * entry = nullptr;

          while (!success && (entry = readdir(dp)) != nullptr)
          {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
              if (sscanf(entry->d_name, "%08x", &packageId) == 1)
              {
                char filename[PATH_MAX];
                sprintf(filename, "%s/%08x", dirname, packageId);

                int fd = open(filename, O_RDONLY | O_CLOEXEC);
                if (fd != -1)
                {
                  if (flock(fd, LOCK_EX | LOCK_NB) == 0)
                  {
                    size_t len = 0;
                    if (read(fd, &len, sizeof(len)) == sizeof(len))
                    {
                      buffer.Resize(len);
                      if (read(fd, buffer.Buf(), len) == static_cast<ssize_t>(len))
                      {
                        success = true;
                      }
                    }
                  }

                  close(fd);
                }

                if (success)
                {
                  unlink(filename);
                }
              }
            }
          }

          closedir(dp);
        }
      }

      if (!success)
      {
        buffer.Resize(0);
      }

      return true;
    }


    bool FileConnection::Wait(int msTimeout)
    {
      Mkdir(this->fepRemote, this->fepLocal);

      char dirname[PATH_MAX];
      GetFolderPath(dirname, this->fepRemote, this->fepLocal);

      auto expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(msTimeout);

      bool pendingPackage = false;

      while (!pendingPackage && std::chrono::steady_clock::now() < expiration)
      {
        uint32_t packageId = 0;
        DIR * dp = opendir(dirname);
        if (dp)
        {
          struct dirent * entry = nullptr;

          while (!pendingPackage && (entry = readdir(dp)) != nullptr)
          {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
              if (sscanf(entry->d_name, "%08x", &packageId) == 1)
              {
                char filename[PATH_MAX];
                sprintf(filename, "%s/%08x", dirname, packageId);

                // check if we are able to open the file
                int fd = open(filename, O_RDONLY | O_CLOEXEC);
                if (fd != -1)
                {
                  if (flock(fd, LOCK_EX | LOCK_NB) == 0)
                  {
                    pendingPackage = true;
                  }

                  close(fd);
                }
              }
            }
          }

          closedir(dp);
        }
      }

      return pendingPackage;
    }


    void FileConnection::GetFolderPath(char * name, const FileEndPoint * to)
    {
      sprintf(name, "%s/%s-%04x", ROOT, to->Name().c_str(), to->Id());
    }


    void FileConnection::GetFolderPath(char * name, const FileEndPoint * from, const FileEndPoint * to)
    {
      sprintf(name, "%s/%s-%04x/%s-%04x", ROOT, to->Name().c_str(), to->Id(), from->Name().c_str(), from->Id());
    }


    void FileConnection::Mkdir(const FileEndPoint * from, const FileEndPoint * to)
    {
      mkdir(ROOT, 0755);

      char name[PATH_MAX];

      GetFolderPath(name, to);

      mkdir(name, 0755);

      GetFolderPath(name, from, to);

      mkdir(name, 0755);
    }
  }
}
