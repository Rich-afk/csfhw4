#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {


  // keep left index and right index
  // start left index 0 right index mid
  // terminate when left hits mid or right hits end

  int sizeLeftArray = mid - begin;
  int sizeRightArray = end - mid;

  int64_t leftArray[sizeLeftArray];
  int64_t rightArray[sizeRightArray];

  for (int i = 0; i < sizeLeftArray; i++) {
    leftArray[i] = arr[sizeLeftArray];
  }
  for (int j = mid; j < end; j++) {
    rightArray[j] = arr[sizeRightArray];
  }

  int i = 0;
  int j = 0;
  int x = begin;
  while(i < sizeLeftArray && j < sizeRightArray) {
    if(leftArray[i] <= rightArray[j]) {
      temparr[x] = leftArray[i];
      i++;
    }
    else {
      temparr[x] = rightArray[j];
      j++;
    }
    x++;
  }

}

int compare_value(const void * a, const void * b) {
    int f = *((int*)a);
    int s = *((int*)b);
    if (f < s) return  1;
    if (f > s) return -1;
    return 0;
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {

  int64_t temp[end];

  // number of elements is at or below the threshold
  if (end - begin <= threshold) {
    // sort the elements sequentially
    // void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
    qsort(arr + begin, end - begin, sizeof(int64_t), compare_value);
  }
  else {
    size_t mid = begin + (end - begin) / 2;
    int64_t *arrTemp = malloc((end - begin) * sizeof(int64_t));
    pid_t pidL = fork();
    pid_t pidR = -1;
    if (pidL == -1) {
	    fprintf(stderr, "fork error");
 	    free(arrTemp);
	    exit(1);
    }
    if (pidL > 0 || pidL < 0) {
	    //fork right half
	    pidR = fork();
	    if (pidR == -1) {
	      fprintf(stderr, "fork error");
 	      free(arrTemp);
	      exit(1);
	    }
	    else if (pidR == 0) {
	      merge_sort(arr, begin, mid, threshold);
	      exit(0);
      }
      else if (pidL == 0) {
        merge_sort(arr, begin, mid, threshold);
        exit(0);
      }
   }
   int wrstatus, wlstatus;
   pid_t r_actual_pid = waitpid(pidR, &wrstatus, 0);
   pid_t l_actual_pid = waitpid(pidL, &wlstatus, 0);

   if (r_actual_pid == -1 || l_actual_pid == -1) {
      fprintf(stderr, "waitpid failure");
      exit(1);
    }

   if (WEXITSTATUS(wrstatus) != 0 || WEXITSTATUS(wlstatus) != 0) {
      fprintf(stderr, "subprocess returned nonzero exit code");
      free(arrTemp);
      exit(1);
    }
   

    // merge the sorted sequences into a temp array
    merge(arr, begin, mid, end, arrTemp);

    // copy the contents of the temp array back to the original array
    memcpy(arr, temp, end);
    free(arrTemp);
    
  }

    
  }
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) fprintf(stderr, "threshold value is invalid");

  // TODO: open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    // file couldn't be opened: handle error and exit
    fprintf(stderr, "file couldn't be opened");
  }

  // TODO: use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
      // handle fstat error and exit
      fprintf(stderr, "fstat error");
  }
  size_t file_size_in_bytes = statbuf.st_size;

  // TODO: map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  // TODO: call close()

  if (data == MAP_FAILED) {
      // handle mmap error and exit
      fprintf(stderr, "mmap error");
  };

  // TODO: sort the data!
  int len = file_size_in_bytes/sizeof(data[0]);
  merge_sort(data, 0, len, threshold);

  // TODO: unmap and close the file
  *data = munmap(NULL, file_size_in_bytes);
    
  if (close(fd)!= 0) {
    fprintf(stderr, "close issue");
    exit(1);
  }


  // TODO: exit with a 0 exit code if sort was successful
  exit(0);
}
