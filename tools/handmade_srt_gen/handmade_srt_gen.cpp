#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#ifdef PLATFORM_WINDOWS
#define VIDEO_DIRECTORY "src\\documents\\videos\\"
#define SLASH '\\'
#define SLASH_STRING "\\"
#else
#define VIDEO_DIRECTORY "src/documents/videos/"
#define SLASH '/'
#define SLASH_STRING "/"
#endif

#define ErrorPrint(...) fprintf(stderr, __VA_ARGS__); exit(1);
#define Print(...) fprintf(stderr, __VA_ARGS__)
#define ArrayCount(A) (sizeof(A) / sizeof((A)[0]))

char GlobalSubFolderName[][128] = {"intro-to-c", "code", "misc", "chat"};

enum mode
{
    // NOTE(wheatdog): This mode will transform all the annotation files in
    // [HandmadeCompanion] to srt format and output them to [output_directory]
    // in specific filenames.
    Mode_Sync,
    // NOTE(wheatdog): This mode will only transform one file to srt format and
    // output the content to stdout.
    Mode_OneFile,
};

void
PrintTimeStamp(FILE *OutputFile, char *TimeStamp)
{
    int TokenArray[3];
    int TokenCount = 0;
    char *Token = strtok(TimeStamp,":");
    while (Token)
    {
        TokenArray[TokenCount++] = atoi(Token);
        Token = strtok(0, ":");
    }

    if (TokenCount == 2)
    {
        TokenArray[2] = TokenArray[1];
        TokenArray[1] = TokenArray[0];
        TokenArray[0] = 0;
    }

    int EndTokenArray[3];
    int CarryOut = 0;
    int ElapsedSecond = 8;
    for (int Index = ArrayCount(EndTokenArray) - 1; Index >= 0; --Index)
    {
        int Result = (TokenArray[Index] +
                      ((Index == ArrayCount(EndTokenArray) - 1)? ElapsedSecond : CarryOut));
        EndTokenArray[Index] = Result % 60;
        CarryOut = Result / 60;
    }

    // TODO(wheatdog): 5 seconds long to display the subtitles
    fprintf(OutputFile, "%02d:%02d:%02d,000 --> %02d:%02d:%02d,000\n",
            TokenArray[0], TokenArray[1], TokenArray[2],
            EndTokenArray[0], EndTokenArray[1], EndTokenArray[2]);
}

void
TransformAndOutput(FILE *InputFile, FILE *OutputFile)
{
    int FindThreeDashTimes = 0;
    char Line[1024];
    int Number = 1;
    while (FindThreeDashTimes != 2)
    {
        if (!fgets(Line, ArrayCount(Line), InputFile))
        {
            break;
        }

        if (strncmp(Line, "---", 2) == 0)
        {
            ++FindThreeDashTimes;
            continue;
        }

        char *TestLine = Line;
        char *QuoteMark[4];
        int QuoteMarkCount = 0;
        while((QuoteMarkCount < 4) && (QuoteMark[QuoteMarkCount] = strchr(TestLine, '\"')))
        {
            TestLine = QuoteMark[QuoteMarkCount] + 1;
            if ((QuoteMarkCount == 3) && (*(QuoteMark[QuoteMarkCount] - 1) == '\\'))
            {
                char *ShiftStart = QuoteMark[QuoteMarkCount]--;
                while (*ShiftStart != '\0')
                {
                    *(ShiftStart - 1) = *ShiftStart;
                    ++ShiftStart;
                }
                *(ShiftStart - 1) = *ShiftStart;

                continue;
            }
            ++QuoteMarkCount;
        }

        if (QuoteMarkCount != 4)
        {
            continue;
        }

        char *TimeStamp = QuoteMark[0] + 1;
        *(QuoteMark[1]) = '\0';
        char *Comment = QuoteMark[2] + 1;
        *(QuoteMark[3]) = '\0';

        fprintf(OutputFile, "%d\n", Number++);
        PrintTimeStamp(OutputFile, TimeStamp);
        fprintf(OutputFile, "%s\n\n", Comment);
    }
}

void
MakeDirectory(char *OutputFileName)
{
#ifdef PLATFORM_WINDOWS
    if (!CreateDirectory(OutputFileName, 0))
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            ErrorPrint("Create directory %s failed\n", OutputFileName);
        }
    }
#else
    if (mkdir(OutputFileName, 0755) < 0)
    {
        if (errno != EEXIST)
        {
            ErrorPrint("mkdir %s failed, errno = %d\n", OutputFileName, errno);
        }
        else
        {
            struct stat FileStatus;
            if (stat(OutputFileName, &FileStatus) < 0)
            {
                ErrorPrint("stat %s failed, errno = %d\n", OutputFileName, errno);
            }

            // TODO(wheatdog): Support symbolic link?
            if (!S_ISDIR(FileStatus.st_mode))
            {
                ErrorPrint("%s existed and it is not a folder.\n", OutputFileName);
            }
        }
    }
#endif
}

void
CheckAndAdjustFolderName(char *Dest, char *FolderName, size_t Size)
{
    strncpy(Dest, FolderName, sizeof(Dest));
    int DestLength = strlen(Dest);
    if (Dest[DestLength - 1] != SLASH)
    {
        Dest[DestLength] = SLASH;
        Dest[++DestLength] = '\0';
    }
}

int
main(int ArgCount, char *Args[])
{
    mode Mode;
    if (ArgCount == 3)
    {
        if (strcmp(Args[1], "-f") == 0)
        {
            Mode = Mode_OneFile;
        }
        else
        {
            Mode = Mode_Sync;
        }
    }
    else
    {
        ErrorPrint("Usage:\n %s [root of HandmadeCompanion] [output_directory]\n %s -f [single_md_file]\n", Args[0], Args[0]);
    }

    switch(Mode)
    {
        case Mode_Sync:
        {
            char OutputFileName[1024];
            CheckAndAdjustFolderName(OutputFileName, Args[2], sizeof(OutputFileName));
            int OutputFileNameLength = strlen(OutputFileName);

            MakeDirectory(OutputFileName);

            char RelativeVideoDir[1024];
            CheckAndAdjustFolderName(RelativeVideoDir, Args[1], sizeof(RelativeVideoDir));
            strncat(RelativeVideoDir, VIDEO_DIRECTORY, sizeof(RelativeVideoDir));
            int EndOfRelativeVideoDir = strlen(RelativeVideoDir);

            for (int FolderIndex = 0;
                 FolderIndex < ArrayCount(GlobalSubFolderName);
                 ++FolderIndex)
            {
                strncat(RelativeVideoDir, GlobalSubFolderName[FolderIndex], sizeof(RelativeVideoDir));
                strncat(RelativeVideoDir, SLASH_STRING, sizeof(RelativeVideoDir));

#ifdef PLATFORM_WINDOWS
                strncat(RelativeVideoDir, "*", sizeof(RelativeVideoDir));
                int EndOfSubFolder = strlen(RelativeVideoDir) - 1;

                WIN32_FIND_DATA FileInfo;
                HANDLE Handle = FindFirstFile(RelativeVideoDir, &FileInfo);
                RelativeVideoDir[EndOfSubFolder] = '\0';

                BOOL FindResult = (Handle != INVALID_HANDLE_VALUE);
                char *FocusFileName = FileInfo.cFileName;
                int KeepGoing = Handle && FindResult;
#else
                strncat(RelativeVideoDir, "/", sizeof(RelativeVideoDir));
                int EndOfSubFolder = strlen(RelativeVideoDir);

                DIR *Dir = opendir(RelativeVideoDir);
                if (!Dir)
                {
                    ErrorPrint("Open directory %s failed\n", RelativeVideoDir);
                }
                struct dirent *DirEntry = readdir(Dir);
                char *FocusFileName = DirEntry->d_name;
                int KeepGoing = (DirEntry != NULL);
#endif
                while (KeepGoing)
                {
                    int FileNameLength = strlen(FocusFileName);
                    if ((FileNameLength > 3) &&
                        (strcmp(FocusFileName + FileNameLength - 2, "md") == 0))
                    {
                        strncat(RelativeVideoDir, FocusFileName, sizeof(RelativeVideoDir));
                        Print("processing: %s\n", RelativeVideoDir);
                        FILE *InputFile = fopen(RelativeVideoDir, "r");
                        if (!InputFile)
                        {
                            ErrorPrint("error: failed to open %s.\n", Args[1]);
                        }

                        char *Day = strtok(FocusFileName,".");
                        char TempBuffer[128];
                        snprintf(TempBuffer, ArrayCount(OutputFileName), "handmade_hero_%s.srt", Day);
                        strncat(OutputFileName, TempBuffer, sizeof(OutputFileName));

                        Print("output    : %s\n", OutputFileName);
                        FILE *OutputFile = fopen(OutputFileName, "w");
                        if (!OutputFile)
                        {
                            ErrorPrint("error: failed to open %s.\n", OutputFileName);
                        }

                        TransformAndOutput(InputFile, OutputFile);

                        fclose(InputFile);
                        fclose(OutputFile);

                        RelativeVideoDir[EndOfSubFolder] = '\0';
                        OutputFileName[OutputFileNameLength] = '\0';
                    }
#ifdef PLATFORM_WINDOWS
                    FindResult = FindNextFile(Handle, &FileInfo);
                    KeepGoing = Handle && FindResult;
                    FocusFileName = FileInfo.cFileName;
#else
                    DirEntry = readdir(Dir);
                    KeepGoing = (DirEntry != NULL);
                    FocusFileName = DirEntry->d_name;
#endif
                }

#ifdef PLATFORM_WINDOWS
                FindClose(Handle);
#else
                closedir(Dir);
#endif
                RelativeVideoDir[EndOfRelativeVideoDir] = '\0';
            }
        } break;

        case Mode_OneFile:
        {
            FILE *InputFile = fopen(Args[2], "r");
            if (!InputFile)
            {
                ErrorPrint("error: failed to open %s.\n", Args[1]);
            }

            TransformAndOutput(InputFile, stdout);

        } break;

        default:
        {
            ErrorPrint("Something strange happened.");
        } break;
    }

    return 0;
}
