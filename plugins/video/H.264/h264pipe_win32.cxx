/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2007 Matthias Schneider, All Rights Reserved
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * Contributor(s): Matthias Schneider (ma30002000@yahoo.de)
 *
 */

/*
  Notes
  -----

 */

#include "h264pipe_win32.h"
#include "shared/rtpframe.h"
#include <string.h>
#include "trace.h"
#include <stdio.h>
#include <tchar.h>
#include <stdio.h> 
#include <tchar.h>


#include <fstream>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096
#define GPL_PROCESS_FILENAME "plugins\\libH264_pwplugin_helper.exe"
#define DIR_SEPERATOR "\\"
#define DIR_TOKENISER ";"

H264EncCtx::H264EncCtx()
{
  width  = 0;
  height = 0;
  size = 0;
  startNewFrame = true;
  loaded = false;  
}

H264EncCtx::~H264EncCtx()
{
  closeAndRemovePipes();
}

bool 
H264EncCtx::Load()
{

  snprintf(pipeName, sizeof(pipeName), "\\\\.\\pipe\\x264-%d", GetCurrentProcessId());

  if (!createPipes()) {
  
    closeAndRemovePipes(); 
    return false;
  }
  
  if (!findGplProcess()) { 

    fprintf(stderr, "H.264 Plugin: Couldn't find GPL process executable: %s\n", GPL_PROCESS_FILENAME); 
    closeAndRemovePipes(); 
    return false;
  }  

  if (!execGplProcess()) { 

	closeAndRemovePipes(); 
    return false;
  }  
  
  if (!ConnectNamedPipe(stream, NULL)) {
	if (GetLastError() != ERROR_PIPE_CONNECTED) {

	  fprintf(stderr, "H.264 Plugin: Could not establish communication with child process (%d)", ErrorMessage());
	  closeAndRemovePipes(); 
	  return false;
	}
  } 

  fprintf(stderr, "H.264 Plugin: Successfully established communication with child process\n");
  return true;
}

void H264EncCtx::call(unsigned msg)
{
  if (msg == H264ENCODERCONTEXT_CREATE) 
     startNewFrame = true;
  writeStream((LPCVOID)&msg, sizeof(msg));
  flushStream();
  readStream((LPVOID)&msg, sizeof(msg));
}

void H264EncCtx::call(unsigned msg, int value)
{
  switch (msg) {
    case SET_FRAME_WIDTH:  width  = value; size = (int) (width * height * 1.5) + sizeof(frameHeader) + 40; break;
    case SET_FRAME_HEIGHT: height = value; size = (int) (width * height * 1.5) + sizeof(frameHeader) + 40; break;
   }
  
  writeStream((LPCVOID) &msg, sizeof(msg));
  writeStream((LPCVOID) &value, sizeof(value));
  flushStream();
  readStream((LPVOID) &msg, sizeof(msg));
}

     
void H264EncCtx::call(unsigned msg , const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned & headerLen, unsigned int & flags, int & ret)
{
  if (startNewFrame) {

    writeStream((LPCVOID) &msg, sizeof(msg));
    if (size) {
      writeStream((LPCVOID) &size, sizeof(size));
      writeStream((LPCVOID) src, size);
      writeStream((LPCVOID) &headerLen, sizeof(headerLen));
      writeStream((LPCVOID) dst, headerLen);

    }
    else {
      writeStream((LPCVOID) &srcLen, sizeof(srcLen));
      writeStream((LPCVOID) src, srcLen);
      writeStream((LPCVOID) &headerLen, sizeof(headerLen));
      writeStream((LPCVOID) dst, headerLen);
    }
  }
  else {
  
    msg = ENCODE_FRAMES_BUFFERED;
    writeStream((LPCVOID) &msg, sizeof(msg));
  }
  
  flushStream();
  
  readStream((LPVOID) &msg, sizeof(msg));
  readStream((LPVOID) &dstLen, sizeof(dstLen));
  readStream((LPVOID) dst, dstLen);
  readStream((LPVOID) &flags, sizeof(flags));
  readStream((LPVOID) &ret, sizeof(ret));

  if (flags & 1) 
    startNewFrame = true;
   else
    startNewFrame = false;
}


bool H264EncCtx::createPipes()
{
  stream = CreateNamedPipe(
           pipeName,
           PIPE_ACCESS_DUPLEX, // FILE_FLAG_FIRST_PIPE_INSTANCE (not supported by minGW lib)
           PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // deny via network ACE
           1,
           BUFSIZE,
           BUFSIZE,
           PIPE_TIMEOUT,
           NULL
  );

  if (stream == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "H.264 Plugin: Failure on creating Pipe - terminating (%s)", ErrorMessage());
    return false;
  }
  return true;
}

void H264EncCtx::closeAndRemovePipes()
{
  if (!DisconnectNamedPipe(stream))
 	TRACE(1, "H264\tIPC\tPP: Failure on disconnecting Pipe (" << ErrorMessage() << ")");
  if (!CloseHandle(stream))
 	TRACE(1, "H264\tIPC\tPP: Failure on closing Handle (" << ErrorMessage() << ")");
}

void H264EncCtx::readStream (LPVOID data, unsigned bytes)
{
  DWORD bytesRead;
  BOOL fSuccess; 
  fSuccess = ReadFile( 
                      stream,     // handle to pipe 
                      data,       // buffer to receive data 
                      bytes,      // size of buffer 
                      &bytesRead, // number of bytes read 
                      NULL        // blocking IO
  );

  if (!fSuccess) {

	TRACE(1, "H264\tIPC\tPP: Failure on reading - terminating (" << ErrorMessage() << ")");
	closeAndRemovePipes();
  }

  if (bytes != bytesRead) {

    TRACE(1, "H264\tIPC\tPP: Failure on reading - terminating (Read " << bytesRead << " bytes, expected " << bytes);
	closeAndRemovePipes();
  }
}

void H264EncCtx::writeStream (LPCVOID data, unsigned bytes)
{
  DWORD bytesWritten;
  BOOL fSuccess; 
  fSuccess = WriteFile( 
                       stream,         // handle to pipe 
                       data,           // buffer to write from 
                       bytes,          // number of bytes to write 
                       &bytesWritten,  // number of bytes written 
                       NULL          // not overlapped I/O 
  );

  if (!fSuccess) {

	TRACE(1, "H264\tIPC\tPP: Failure on writing - terminating (" << ErrorMessage() << ")");
	closeAndRemovePipes();
  }

  if (bytes != bytesWritten) {

    TRACE(1, "H264\tIPC\tPP: Failure on writing - terminating (Written " << bytesWritten << " bytes, intended " << bytes);
	closeAndRemovePipes();
  }
}

void H264EncCtx::flushStream ()
{
  if (!FlushFileBuffers(stream)) {

	TRACE(1, "H264\tIPC\tPP: Failure on flushing - terminating (" << ErrorMessage() << ")");
	closeAndRemovePipes();
  }
}

bool H264EncCtx::findGplProcess()
{
  char * env = ::getenv("PWLIBPLUGINDIR");
  if (env != NULL) {
    const char * token = strtok(env, DIR_TOKENISER);
    while (token != NULL) {

      if (checkGplProcessExists(token)) 
        return true;

      token = strtok(NULL, DIR_TOKENISER);
    }
  }
  return checkGplProcessExists(".");
}

bool H264EncCtx::checkGplProcessExists (const char * dir)
{
  fstream fin;
  memset(gplProcess, 0, sizeof(gplProcess));
  strncpy(gplProcess, dir, sizeof(gplProcess));
  if (gplProcess[strlen(gplProcess)-1] != DIR_SEPERATOR[0]) 
    strcat(gplProcess, DIR_SEPERATOR);
  strcat(gplProcess, GPL_PROCESS_FILENAME);

  fin.open(gplProcess,ios::in);
  if( !fin.is_open() ){
    fprintf(stderr, "H.264 Plugin: Couldn't find GPL process executable in %s\n", gplProcess); 
    fin.close();
    return false;
  }
  fin.close();
  fprintf(stderr, "H.264 Plugin: Found GPL process executable in %s\n", gplProcess); 
  return true;
}

bool H264EncCtx::execGplProcess() 
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char command[1024];

  snprintf(command,sizeof(command), "%s %s", gplProcess, pipeName);
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  // Start the child process. 
  if( !CreateProcess( NULL,        // No module name (use command line)
                      command,  // Command line
                      NULL,        // Process handle not inheritable
                      NULL,        // Thread handle not inheritable
                      FALSE,       // Set handle inheritance to FALSE
                      0,           // No creation flags
                      NULL,        // Use parent's environment block
                      NULL,        // Use parent's starting directory 
                      &si,         // Pointer to STARTUPINFO structure
                      &pi ))       // Pointer to PROCESS_INFORMATION structure
  {
      fprintf(stderr, "H.264 Plugin: Couldn't create child process: %s\n", ErrorMessage()); 
      return false;
  }
  fprintf(stderr, "H.264 Plugin: Successfully created child process %d\n", pi.dwProcessId);
  return true;
}

const char* 
H264EncCtx::ErrorMessage()
{
  static char string [1024];
  DWORD dwMsgLen;

  memset (string, 0, sizeof (string));
  dwMsgLen = FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL,
                             GetLastError (),
                             MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                             (LPSTR) string,
                             sizeof (string)-1,
                             NULL);
  if (dwMsgLen) {
    string [ strlen(string) - 2 ] = 0;
    snprintf (string, sizeof (string), "%s (%u)", string, (int) GetLastError ());
  }
  else {
    snprintf (string, sizeof (string), "%u", (int) GetLastError ());
  }

  return string;
}
