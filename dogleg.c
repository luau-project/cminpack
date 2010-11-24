/* dogleg.f -- translated by f2c (version 20020621).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <math.h>
#include "cminpack.h"
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))

/* Table of constant values */

/* Subroutine */ void dogleg(int n, const double *r__, int lr, 
	const double *diag, const double *qtb, double delta, double *x, 
	double *wa1, double *wa2)
{
    /* System generated locals */
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    int i__, j, k, l, jj, jp1;
    double sum, temp, alpha, bnorm;
    double gnorm, qnorm, epsmch;
    double sgnorm;

/*     ********** */

/*     subroutine dogleg */

/*     given an m by n matrix a, an n by n nonsingular diagonal */
/*     matrix d, an m-vector b, and a positive number delta, the */
/*     problem is to determine the convex combination x of the */
/*     gauss-newton and scaled gradient directions that minimizes */
/*     (a*x - b) in the least squares sense, subject to the */
/*     restriction that the euclidean norm of d*x be at most delta. */

/*     this subroutine completes the solution of the problem */
/*     if it is provided with the necessary information from the */
/*     qr factorization of a. that is, if a = q*r, where q has */
/*     orthogonal columns and r is an upper triangular matrix, */
/*     then dogleg expects the full upper triangle of r and */
/*     the first n components of (q transpose)*b. */

/*     the subroutine statement is */

/*       subroutine dogleg(n,r,lr,diag,qtb,delta,x,wa1,wa2) */

/*     where */

/*       n is a positive integer input variable set to the order of r. */

/*       r is an input array of length lr which must contain the upper */
/*         triangular matrix r stored by rows. */

/*       lr is a positive integer input variable not less than */
/*         (n*(n+1))/2. */

/*       diag is an input array of length n which must contain the */
/*         diagonal elements of the matrix d. */

/*       qtb is an input array of length n which must contain the first */
/*         n elements of the vector (q transpose)*b. */

/*       delta is a positive input variable which specifies an upper */
/*         bound on the euclidean norm of d*x. */

/*       x is an output array of length n which contains the desired */
/*         convex combination of the gauss-newton direction and the */
/*         scaled gradient direction. */

/*       wa1 and wa2 are work arrays of length n. */

/*     subprograms called */

/*       minpack-supplied ... dpmpar,enorm */

/*       fortran-supplied ... dabs,dmax1,dmin1,dsqrt */

/*     argonne national laboratory. minpack project. march 1980. */
/*     burton s. garbow, kenneth e. hillstrom, jorge j. more */

/*     ********** */
    /* Parameter adjustments */
    --wa2;
    --wa1;
    --x;
    --qtb;
    --diag;
    --r__;

    /* Function Body */

/*     epsmch is the machine precision. */

    epsmch = dpmpar(1);

/*     first, calculate the gauss-newton direction. */

    jj = n * (n + 1) / 2 + 1;
    for (k = 1; k <= n; ++k) {
	j = n - k + 1;
	jp1 = j + 1;
	jj -= k;
	l = jj + 1;
	sum = 0.;
	if (n >= jp1) {
            for (i__ = jp1; i__ <= n; ++i__) {
                sum += r__[l] * x[i__];
                ++l;
            }
        }
	temp = r__[jj];
	if (temp == 0.) {
            l = j;
            for (i__ = 1; i__ <= j; ++i__) {
                /* Computing MAX */
                d__2 = fabs(r__[l]);
                temp = max(temp,d__2);
                l = l + n - i__;
            }
            temp = epsmch * temp;
            if (temp == 0.) {
                temp = epsmch;
            }
        }
	x[j] = (qtb[j] - sum) / temp;
    }

/*     test whether the gauss-newton direction is acceptable. */

    for (j = 1; j <= n; ++j) {
	wa1[j] = 0.;
	wa2[j] = diag[j] * x[j];
    }
    qnorm = enorm(n, &wa2[1]);
    if (qnorm <= delta) {
        return;
    }

/*     the gauss-newton direction is not acceptable. */
/*     next, calculate the scaled gradient direction. */

    l = 1;
    for (j = 1; j <= n; ++j) {
	temp = qtb[j];
	for (i__ = j; i__ <= n; ++i__) {
	    wa1[i__] += r__[l] * temp;
	    ++l;
	}
	wa1[j] /= diag[j];
    }

/*     calculate the norm of the scaled gradient and test for */
/*     the special case in which the scaled gradient is zero. */

    gnorm = enorm(n, &wa1[1]);
    sgnorm = 0.;
    alpha = delta / qnorm;
    if (gnorm != 0.) {

/*     calculate the point along the scaled gradient */
/*     at which the quadratic is minimized. */

        for (j = 1; j <= n; ++j) {
            wa1[j] = wa1[j] / gnorm / diag[j];
        }
        l = 1;
        for (j = 1; j <= n; ++j) {
            sum = 0.;
            for (i__ = j; i__ <= n; ++i__) {
                sum += r__[l] * wa1[i__];
                ++l;
            }
            wa2[j] = sum;
        }
        temp = enorm(n, &wa2[1]);
        sgnorm = gnorm / temp / temp;

/*     test whether the scaled gradient direction is acceptable. */

        alpha = 0.;
        if (sgnorm < delta) {

/*     the scaled gradient direction is not acceptable. */
/*     finally, calculate the point along the dogleg */
/*     at which the quadratic is minimized. */

            bnorm = enorm(n, &qtb[1]);
            temp = bnorm / gnorm * (bnorm / qnorm) * (sgnorm / delta);
            /* Computing 2nd power */
            d__1 = sgnorm / delta;
            /* Computing 2nd power */
            d__2 = temp - delta / qnorm;
            /* Computing 2nd power */
            d__3 = delta / qnorm;
            /* Computing 2nd power */
            d__4 = sgnorm / delta;
            temp = temp - delta / qnorm * (d__1 * d__1)
                   + sqrt(d__2 * d__2
                          + (1. - d__3 * d__3) * (1. - d__4 * d__4));
            /* Computing 2nd power */
            d__1 = sgnorm / delta;
            alpha = delta / qnorm * (1. - d__1 * d__1) / temp;
        }
    }

/*     form appropriate convex combination of the gauss-newton */
/*     direction and the scaled gradient direction. */

    temp = (1. - alpha) * min(sgnorm,delta);
    for (j = 1; j <= n; ++j) {
	x[j] = temp * wa1[j] + alpha * x[j];
    }

/*     last card of subroutine dogleg. */

} /* dogleg_ */

