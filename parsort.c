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

int is_sorted(int64_t *arr, size_t begin, size_t end) {
  for (size_t i = begin + 1; i < end; i++) {
    if (arr[i] < arr[i-1])
      return 0;
  }
  return 1;
}


void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {

  // keep left index and right index
  // start left index 0 right index mid
  // terminate when left hits mid or right hits end

  int i = begin;
  int j = mid;
  int x = begin;
  while(i < mid && j < end) {
    if(arr[i] <= arr[j]) {
      temparr[x] = arr[i];
      i++;
    }
    else {
      temparr[x] = arr[j];
      j++;
    }
    x++;
  }

  while(i < mid) {
    temparr[x] = arr[i];
    i++;
    x++;
  }

  while(j < end) {
    temparr[x] = arr[j];
    j++;
    x++;
  }

  for(int i = begin; i < end; i++) {
    arr[i] = temparr[i];
  }
}

int compare_value(const void * a, const void * b) {
    int64_t f = *(int64_t*)a;
    int64_t s = *(int64_t*)b;
    if (f < s) return -1;
    if (f > s) return 1;
    return 0;
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {

  if (end <= begin) {
    fprintf(stderr, "invalid begining and end values");
  }

  // number of elements is at or below the threshold
  if (end - begin <= threshold) {
    // sort the elements sequentially
    // void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
    qsort(arr + begin, end - begin, sizeof(int64_t), compare_value);
    // if(!(is_sorted(arr + begin, end - begin, end))) {
    //   printf("qsort %lu %lu\n", begin, end - begin);
    // }
  }
  else {
    size_t mid = begin + (end - begin) / 2;
    int64_t *arrTemp = malloc((end - begin) * sizeof(int64_t));
    pid_t pidL = fork();
    if (pidL == -1) {
	    fprintf(stderr, "fork error");
 	    free(arrTemp);
	    exit(1);
    }
    if (pidL == 0) {
      merge_sort(arr, begin, mid, threshold);
      exit(0);
    }
    pid_t pidR = fork();
    if (pidR == -1) {
	    fprintf(stderr, "fork error");
 	    free(arrTemp);
	    exit(1);
    }
    if (pidR == 0) {
      merge_sort(arr, mid, end, threshold);
      exit(0);
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
    //memcpy(arr, temp, end);
    free(arrTemp);
    
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
  if (end != argv[2] + strlen(argv[2])) {
    fprintf(stderr, "threshold value is invalid");
    exit(1);
  }

  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    // file couldn't be opened: handle error and exit
    fprintf(stderr, "file couldn't be opened");
    exit(1);
  }

  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
      // handle fstat error and exit
      fprintf(stderr, "fstat error");
      exit(1);
  }
  size_t file_size_in_bytes = statbuf.st_size;

  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (data == MAP_FAILED) {
      // handle mmap error and exit
      fprintf(stderr, "mmap error");
      exit(1);
  };

  int len = file_size_in_bytes/sizeof(data[0]);
  merge_sort(data, 0, len, threshold);

  munmap(data, file_size_in_bytes);
    
  if (close(fd)!= 0) {
    fprintf(stderr, "close issue");
    exit(1);
  }

  exit(0);
}
