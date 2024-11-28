#ifndef LN_H
#define LN_H

#include <stdlib.h>
#include "./fconsts.h"
#include <math.h>

float Ln(float x)
/******************************************************************************/
/*
  Purpose:

    LN_CORDIC evaluates the natural logarithm using the CORDIC method.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    19 January 2012

  Author:

    John Burkardt

  Reference:

    Frederick Ruckdeschel,
    BASIC Scientific Subroutines,
    Volume II,
    McGraw-Hill, 1980,
    ISBN: 0-07-054202-3,
    LC: QA76.95.R82.

  Parameters:

    Input, float X, the argument.

    Input, int N, the number of steps to take.

    Output, float FX, the natural logarithm of X.

  Local Parameters:

    Local, float A(1:25) = exp ( (1/2)^(1:25) );
*/
{
  float a[] = {
      1.648721270700128,
      1.284025416687742,
      1.133148453066826,
      1.064494458917859,
      1.031743407499103,
      1.015747708586686,
      1.007843097206488,
      1.003913889338348,
      1.001955033591003,
      1.000977039492417,
      1.000488400478694,
      1.000244170429748,
      1.000122077763384,
      1.000061037018933,
      1.000030518043791,
      1.0000152589054785,
      1.0000076294236351,
      1.0000038147045416,
      1.0000019073504518,
      1.0000009536747712};
  int k = 0;
  float poweroftwo = 0.0;
  float w[20] ; // matrix 10 posições iniciadas em 0

  if (x <= 0.0)
  {
    return -INFINITY;
  }
  while (FM_E <= x)
  {
    k = k + 1;
    x = x / FM_E;
  }

  while (x < 1.0)
  {
    k = k - 1;
    x = x * FM_E;
  }

  int i = 0;
  while (i < 20)
  {
    w[i] = 0.0;
    if (a[i] < x)
    {
      w[i] = 1.0;
      x = x / a[i];
    }
    i++;
  }

  x -= 1.0;

  x = x * (1.0 - (x / 2.0) * (1.0 + (x / 3.0) * (1.0 - x / 4.0)));

  poweroftwo = 0.5;
  i = 0;
  while (i < 20)
  {
    x = x + w[i] * poweroftwo;
    poweroftwo = poweroftwo / 2.0;
    i++;
  }
  return x + (float)(k);
}

#endif