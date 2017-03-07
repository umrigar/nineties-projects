/* Evaluate nth-degree polynomial 
 * coeffs[n]*point^n + coeffs[n-1]*point^(n-1) + ... + coeffs[0]
 */
double evalPoly(double point, double coeffs[], int n)
{
  double sum= 0;
  int i;
  for (i= n; i >= 0; i--) sum= sum*point + coeffs[i];
  return sum;
}

