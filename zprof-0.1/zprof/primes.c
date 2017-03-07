/* Program from Jon Bentley, "More Programming Pearls", pg. 3. */

static int
prime(int n) 
{
  int i;
  for (i= 2; i < n; i++) {
    if (n % i == 0) {
      return 0;
    }
  }
  return 1;
}

int 
main()
{
  const int n= 1000;
  int i;
  for (i= 2; i <= n; i++) {
    if (prime(i)) {
      printf("%d\n", i);
    }
  }
  return 0;
}
