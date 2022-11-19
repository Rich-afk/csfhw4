# CONTRIBUTIONS

We both worked equally on this homework.

# REPORT

1. Indicate the amount of time that your parsort program took so sort the test data for each threshold value

test_16M.in 2097152
real    0m0.415s
user    0m0.392s
sys     0m0.020s

test_16M.in 1048576
real    0m0.268s
user    0m0.458s
sys     0m0.030s

test_16M.in 524288
real    0m0.171s
user    0m0.465s
sys     0m0.043s

test_16M.in 262144
real    0m0.130s
user    0m0.475s
sys     0m0.057s

test_16M.in 131072
real    0m0.139s
user    0m0.527s
sys     0m0.095s

test_16M.in 65536
real    0m0.141s
user    0m0.518s
sys     0m0.123s

test_16M.in 32768
real    0m0.145s
user    0m0.514s
sys     0m0.162s

test_16M.in 16384
real    0m0.148s
user    0m0.526s
sys     0m0.193s


2. States a reasonable explanation for why you saw the times you did

By parallelizing the recursive call on our merge sort process, we are able to make full use of multiple CPU cores, such that the real time decreases with threshold.
