#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ErrorPrint(...) fprintf(stderr, __VA_ARGS__); exit(1);
#define Print(...) fprintf(stderr, __VA_ARGS__)
#define ArrayCount(A) (sizeof(A) / sizeof((A)[0]))

#define VIDEO_DIRECTORY "src\\documents\\videos\\"

char GlobalSubFolderName[][128] = {"intro-to-c", "win32-platform", "game-architecture", "misc"};

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

    // TODO(wheatdog): 5 seconds long to display the subtitles
    fprintf(OutputFile, "%02d:%02d:%02d,000 --> %02d:%02d:%02d,000\n",
          TokenArray[0], TokenArray[1], TokenArray[2],
          TokenArray[0], TokenArray[1], TokenArray[2]+5);
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
            strncpy(OutputFileName, Args[2], sizeof(OutputFileName));
            int OutputFileNameLength = strlen(OutputFileName);
            if (OutputFileName[OutputFileNameLength - 1] != '\\')
            {
                OutputFileName[OutputFileNameLength] = '\\';
                OutputFileName[++OutputFileNameLength] = '\0';
            }

            BOOL CreateDirResult;
            if ((CreateDirResult = CreateDirectory(OutputFileName, 0)) == 0)
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    ErrorPrint("Create directory %s failed", OutputFileName);
                }
            }

            char RelativeVideoDir[1024];
            strncpy(RelativeVideoDir, Args[1], sizeof(RelativeVideoDir));
            int RelativeVideoDirLength = strlen(RelativeVideoDir);
            if (RelativeVideoDir[RelativeVideoDirLength - 1] != '\\')
            {
                RelativeVideoDir[RelativeVideoDirLength] = '\\';
                RelativeVideoDir[++RelativeVideoDirLength] = '\0';
            }
            strncat(RelativeVideoDir, VIDEO_DIRECTORY, sizeof(RelativeVideoDir));
            int EndOfRelativeVideoDir = strlen(RelativeVideoDir);

            for (int FolderIndex = 0;
                 FolderIndex < ArrayCount(GlobalSubFolderName);
                 ++FolderIndex)
            {
                strncat(RelativeVideoDir, GlobalSubFolderName[FolderIndex], sizeof(RelativeVideoDir));
                strncat(RelativeVideoDir, "\\*", sizeof(RelativeVideoDir));
                int EndOfSubFolder = strlen(RelativeVideoDir) - 1;

                WIN32_FIND_DATA FileInfo;
                HANDLE Handle = FindFirstFile(RelativeVideoDir, &FileInfo);
                RelativeVideoDir[EndOfSubFolder] = '\0';

                BOOL FindResult = (Handle != INVALID_HANDLE_VALUE);
                while (Handle && FindResult)
                {
                    int FileNameLength = strlen(FileInfo.cFileName);
                    if (strcmp(FileInfo.cFileName + FileNameLength - 2, "md") == 0)
                    {
                        strncat(RelativeVideoDir, FileInfo.cFileName, sizeof(RelativeVideoDir));
                        Print("processing: %s\n", RelativeVideoDir);
                        FILE *InputFile = fopen(RelativeVideoDir, "r");
                        if (!InputFile)
                        {
                            ErrorPrint("error: failed to open %s.\n", Args[1]);
                        }

                        char *Day = strtok(FileInfo.cFileName,".");
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
                    FindResult = FindNextFile(Handle, &FileInfo);
                }

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
