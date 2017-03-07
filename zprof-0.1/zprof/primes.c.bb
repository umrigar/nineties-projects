         /* Program from Jon Bentley, "More Programming Pearls", pg. 3. */
         
         static int
         prime(int n) 
         {
           int i;
999        for (i= 2; i < n; i++) {
78022        if (n % i == 0) {
831            return 0;
             }
           }
168        return 1;
         }
         
         int 
         main()
         {
           const int n= 1000;
           int i;
1          for (i= 2; i <= n; i++) {
999          if (prime(i)) {
168            printf("%d\n", i);
             }
           }
1          return 0;
         }
