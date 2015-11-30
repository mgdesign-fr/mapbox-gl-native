#include "util.hpp"

#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/crc.hpp>
#pragma GCC diagnostic pop

#include <csignal>

#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace mbgl {
namespace test {

#ifdef _WIN32

static void ErrorExit(TCHAR const* lpszFunction) 
{ 
    printf("ERROR - %s ; GetLastError = %d", lpszFunction, GetLastError()); fflush(stdout);
    exit(1);
}

// TODO
pid_t startServer(const char *executable)
{
    printf("startServer '%s' on _WIN32\n", executable); fflush(stdout);

    // from https://msdn.microsoft.com/en-us/library/ms682499(VS.85).aspx

    char cmdLine[MAX_PATH+1];
    sprintf(cmdLine, "node \"%s\"", executable);

    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr; 
 
    // Set the bInheritHandle flag so pipe handles are inherited. 

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    // Create a pipe for the child process's STDOUT. 

    if ( ! CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0) ) 
        ErrorExit(TEXT("StdoutRd CreatePipe")); 

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if ( ! SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
        ErrorExit(TEXT("Stdout SetHandleInformation")); 

    // Create a pipe for the child process's STDIN. 

    if (! CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) 
        ErrorExit(TEXT("Stdin CreatePipe")); 

    // Ensure the write handle to the pipe for STDIN is not inherited. 

    if ( ! SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
        ErrorExit(TEXT("Stdin SetHandleInformation")); 

    //

    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE; 
 
    // Set up members of the PROCESS_INFORMATION structure. 
 
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.
 
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
    // Create the child process. 
    
    bSuccess = CreateProcess(NULL, 
        cmdLine,       // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 
   
    // If an error occurs, exit the application. 
    if ( ! bSuccess ) {
        ErrorExit(TEXT("CreateProcess"));
    } 
    else {
        // Close handles to the child process and its primary thread.
        // Some applications might keep these handles to monitor the status
        // of the child process, for example. 
        //CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }

    // Read output from the child process's pipe for STDOUT
    // and write to the parent process's pipe for STDOUT. 
    // Stop when there is no more data. 
    DWORD dwRead; 
    CHAR chBuf[8]; 
    bSuccess = FALSE;

    bSuccess = ReadFile( hChildStd_OUT_Rd, chBuf, 8, &dwRead, NULL);
    if( ! bSuccess || dwRead == 0 ) ErrorExit(TEXT("ReadFile from pipe"));

    //
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    return piProcInfo.hProcess;
}    

// TODO
void stopServer(pid_t pid)
{
    printf("stopServer on _WIN32\n"); fflush(stdout);
    TerminateProcess(pid, 1);
    CloseHandle(pid);
}

#else

pid_t startServer(const char *executable) {
    int pipefd[2];

    if (pipe(pipefd)) {
        throw std::runtime_error("Cannot create server pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        Log::Error(Event::Setup, "Cannot create server process");
        exit(1);
    } else if (pid == 0) {
        // This is the child process.

        // Close the input side of the pipe.
        close(pipefd[0]);

        // Launch the actual server process, with the pipe ID as the first argument.
        char *args[] = { const_cast<char *>(executable),
                         const_cast<char *>(std::to_string(pipefd[1]).c_str()),
                         nullptr };
        int ret = execv(executable, args);
        // This call should not return. In case execve failed, we exit anyway.
        if (ret < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
        }
        exit(0);
    } else {
        // This is the parent process.

        // Close output side of the pipe.
        close(pipefd[1]);

        // Wait until the server process closes the handle.
        char buffer[2];
        ssize_t bytes = 0, total = 0;
        while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) != 0) {
            total += bytes;
        }
        if (bytes < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
            exit(1);
        }
        if (total != 2 || strncmp(buffer, "OK", 2) != 0) {
            Log::Error(Event::Setup, "Failed to start server");
            exit(1);
        }
        close(pipefd[0]);
    }
    return pid;
}

void stopServer(pid_t pid) {
    kill(pid, SIGTERM);
}

#endif

// from https://gist.github.com/ArtemGr/997887
uint64_t crc64(const char* data, size_t size) {
    boost::crc_optimal<64, 0x04C11DB7, 0, 0, false, false> crc;
    crc.process_bytes(data, size);
    return crc.checksum();
}

uint64_t crc64(const std::string& str) {
    return crc64(str.data(), str.size());
}

}
}
