#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define THREADCOUNT 3

/// @brief Strut to define parameters and return values for threads
typedef struct
{
  struct arg_val_t
  {
    char *stringChunk;
  } arg_val;
  struct ret_val_t
  {
    char *encodedString;
  } ret_val;
} arg_t;

/// @brief Write to output when Single Threading was used
/// @param count
/// @param currentCharacter
void writeToOutput(int count, char currentCharacter)
{
  unsigned char bytes[5];
  bytes[4] = currentCharacter;
  for (int i = 0; i < 4; i++)
  {
    bytes[i] = (count >> (i * 8)) & 0xFF; // Convert decimal to binary and store each byte (4 total) in array
  }
  fwrite(bytes, 1, 5, stdout);
}

/// @brief RLE Encoding for single threading
/// @param inputFile
void encodeRLE(FILE *inputFile)
{
  char currentChar, nextChar;
  int count = 1;
  currentChar = getc(inputFile); // Read first character
  // Loop through the file until EOF is reached
  while (currentChar != EOF)
  {
    nextChar = getc(inputFile); // Read Next Character
    if (nextChar == currentChar)
    {
      count++;
    }
    else
    {
      writeToOutput(count, currentChar); // Write Binary Output
      count = 1;                         // Reset count for new character
    }
    currentChar = nextChar; // Move to next character
  }
}

/// @brief Write to output when multi-threading was used.
/// @param rleString
void writeThreadOutput(char *rleString)
{
  unsigned char bytes[5];

  // Parse the RLE-encoded string
  int len = strlen(rleString);
  int i = 0;
  while (i < len)
  {
    // Extract the count
    int count = atoi(&rleString[i]);
    // Move to the character
    while (rleString[i] >= '0' && rleString[i] <= '9')
    {
      i++;
    }
    // Extract the character
    char currentCharacter = rleString[i];
    bytes[4] = currentCharacter;

    // Write the count and character to output
    for (int j = 0; j < 4; j++)
    {
      bytes[j] = (count >> (j * 8)) & 0xFF;
    }
    fwrite(bytes, 1, 5, stdout);

    // Move to the next count or character
    i++;
  }
  free(rleString);
}

/// @brief RLE encoding for multithreading
/// @param inputString
/// @return string logically encoded ex. "4a5b8c"
char *encodeThreadsRLE(char *inputString)
{
  char currentChar = '0', nextChar = '0';
  int count = 1, outputIndex = 0;
  currentChar = inputString[0]; // Read first character
  int stringLength = 0;
  stringLength = strlen(inputString);
  char *encodedOutput = (char *)calloc((2 * stringLength + 1), sizeof(char));

  // Loop through the string until the null terminator is reached
  for (int i = 1; inputString[i] != '\0'; i++)
  {
    nextChar = inputString[i]; // Read next character
    if (nextChar == currentChar)
    {
      count++;
    }
    else
    {
      outputIndex += sprintf(encodedOutput + outputIndex, "%d%c", count, currentChar);
      count = 1; // Reset count for new character
    }
    currentChar = nextChar; // Move to next character
  }
  outputIndex += sprintf(encodedOutput + outputIndex, "%d%c", count, currentChar);
  encodedOutput[outputIndex] = '\0';
  return encodedOutput;
}

/// @brief Function executed by each thread
/// @param arg
/// @return NULL
void *threadFunction(void *arg)
{
  arg_t *argStructs = (arg_t *)arg; // Take in thread structure
                                    // Get arguments from passed in parameter
  char *stringChunk = argStructs->arg_val.stringChunk;
  argStructs->ret_val.encodedString = encodeThreadsRLE(stringChunk);
  free(stringChunk);
  return NULL;
}

/// @brief Get file size
/// @param file
/// @return File size as int
int getFileSize(FILE *file)
{
  struct stat st;
  int fileDescriptor = fileno(file);
  fstat(fileDescriptor, &st);
  int fileSize = st.st_size;
  return fileSize;
}

/// @brief Split file into separate chunks to be decoded
/// @param input
/// @param chunkNum
/// @param chunkStartingIndex
/// @return ending index of line
int defineChunks(char *input, char *chunkNum, int chunkStartingIndex)
{
  int length = strlen(input);
  int currentIndex = 0;
  char previousChar;
  char currentChar = input[chunkStartingIndex];

  while (1)
  {
    currentChar = input[chunkStartingIndex + currentIndex];

    if ((currentIndex >= length / 3 && currentChar != previousChar) ||
        chunkStartingIndex + currentIndex >= length)
    {
      break;
    }

    chunkNum[currentIndex] = currentChar;

    previousChar = currentChar;

    currentIndex++;
  }
  chunkNum[currentIndex] = '\0';
  return (chunkStartingIndex + currentIndex); // Ending index
}

/// @brief Decode file with threads
/// @param inputFile
/// @param fileSize
void threadDecode(FILE *inputFile, int fileSize)
{
  char *string = mmap(0, fileSize, PROT_READ, MAP_PRIVATE, fileno(inputFile), 0); // Map file content to contiguous chunk of memory
  int stringLength = strlen(string);
  int sectionLength = stringLength / 3;

  char *chunk1 = malloc(stringLength * sizeof(char));
  char *chunk2 = malloc(stringLength * sizeof(char));
  char *chunk3 = malloc(stringLength * sizeof(char));

  char *chunkArray[3] = {chunk1, chunk2, chunk3};

  int chunk2StartingIndex = defineChunks(string, chunk1, 0);
  int chunk3StartingIndex = defineChunks(string, chunk2, chunk2StartingIndex);
  defineChunks(string, chunk3, chunk3StartingIndex);

  munmap(string, fileSize); // Deallocate memory from mmap

  pthread_t threads[THREADCOUNT];
  arg_t threadParameter[THREADCOUNT];

  // Create threads, each responsible for processing a section of the string
  for (int threadNum = 0; threadNum < THREADCOUNT; threadNum++)
  {
    threadParameter[threadNum].arg_val.stringChunk = chunkArray[threadNum];
    pthread_create(&threads[threadNum], NULL, threadFunction, (void *)&threadParameter[threadNum]);
  }
  // Wait for all threads to complete
  for (int waitingThread = 0; waitingThread < THREADCOUNT; waitingThread++)
  {
    pthread_join(threads[waitingThread], NULL);
    writeThreadOutput(threadParameter[waitingThread].ret_val.encodedString);
  }


}

/// @brief Loop files given through args, test size, and encode accordingly
/// @param inputFile
/// @param argNum
/// @param arguments
void loopFiles(int argNum, char *arguments[])
{
  FILE *inputFile;
  char *concatenatedString = NULL;
  int index = 0, totalSize = 0;
  for (int argIndex = 1; argIndex < argNum; argIndex++)
  {
    inputFile = fopen(arguments[argIndex], "rb"); // Open File
    int fileSize = getFileSize(inputFile);        // Get file Size
    totalSize += fileSize;
    if (fileSize > 4096)
    {                                    // Use threads if size is greater than 4096 bytes
      threadDecode(inputFile, fileSize); // Decode with threads
      fclose(inputFile);
    }
    else
    { // File under 4096 bytes
      concatenatedString = realloc(concatenatedString, totalSize + 1);
      int inputCharacter;
      while ((inputCharacter = fgetc(inputFile)) != EOF)
      {
        concatenatedString[index] = inputCharacter;
        index++;
      }

      if (argIndex == argNum - 1)
      { // If at end of files under 4096 bytes
        concatenatedString[index] = '\0';

        char* encodedString = encodeThreadsRLE(concatenatedString);
        writeThreadOutput(encodedString); // Encode with combied file content
        encodedString = NULL; free(encodedString);
        free(concatenatedString);
      }
      fclose(inputFile);
    }
  }
}

/// @brief Main Function
/// @param argc
/// @param args
/// @return success value
int main(int argc, char *args[])
{
  if (argc == 1)
  { // File was not given
    printf("wzip: file1 [file2 ...]\n");
    return 1;
  }

  if (argc > 1)
  { // Check multiple files
    loopFiles(argc, args);
  }
  else
  { // Encode Single File
    FILE *inputFile;
    inputFile = fopen(args[1], "rb");
    encodeRLE(inputFile); // Single Thread Encoding
    fclose(inputFile);
  }
  return 0;
}
