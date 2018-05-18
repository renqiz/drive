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

#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "network/FileConnection.h"
#include "network/FileServer.h"

namespace dfs
{
  namespace network
  {
    FileServer::~FileServer()
    {
      this->Shutdown();

      if (this->thread.joinable())
      {
        this->thread.join();
      }
    }


    bool FileServer::Listen(const IEndPoint * local)
    {
      auto fep = dynamic_cast<const FileEndPoint *>(local);
      if (!fep)
      {
        return false;
      }

      this->localEndPoint = *fep;

      this->running = true;

      this->thread = std::thread(std::bind(&FileServer::ThreadProc, this));
    }


    void FileServer::Shutdown()
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      this->running = false;
      this->cond.notify_all();
    }


    void FileServer::ThreadProc()
    {
      char dirname[PATH_MAX];
      FileConnection::GetFolderPath(dirname, &this->localEndPoint);

      while (this->running)
      {
        this->CheckConnections(dirname);

        for (auto & pair : this->connections)
        {
          this->CheckRequests(pair.second);
        }

        {
          std::unique_lock<std::mutex> lock(this->mutex);
          this->cond.wait_for(lock, std::chrono::milliseconds(50));
        }
      }
    }


    void FileServer::CheckConnections(const char * dirname)
    {
      DIR * dp = opendir(dirname);
      if (!dp)
      {
        return;
      }

      struct dirent * entry = nullptr;
      while ((entry = readdir(dp)) != nullptr)
      {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
          char name[PATH_MAX];
          unsigned id = 0;
          if (sscanf(entry->d_name, "%[^-]-%04x", name, &id) == 2)
          {
            auto remoteEp = new FileEndPoint(name, id);
            auto remote = std::unique_ptr<IEndPoint>(remoteEp);
            if (this->connections.find(*remoteEp) == this->connections.end())
            {
              auto local = std::unique_ptr<IEndPoint>(new FileEndPoint(this->localEndPoint));
              auto conn = new FileConnection(std::move(local), std::move(remote));
              this->connections[*remoteEp] = conn;
              if (this->Handler())
              {
                this->Handler()->OnNewConnection(conn);
              }
            }
          }
        }
      }

      // TODO: cleanup connections

      closedir(dp);
    }


    void FileServer::CheckRequests(Connection * conn)
    {
      Buffer buffer;

      while (conn->Receive(buffer))
      {
        if (buffer.Size() == 0)
        {
          break;
        }

        if (this->Handler())
        {
          this->Handler()->OnRequest(conn, buffer.Buf(), buffer.Size());
        }
      }
    }
  }
}
