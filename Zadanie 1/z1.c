#include <stdio.h>
#include <math.h>

double f1(double x)
{
    if (x < 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
double f2(double x)
{
    return 1/(1 + exp(-x));
}
double f3(double x)
{
    return (exp(x) - exp(-x))/(exp(x) + exp(-x));

}
double f4(double x)
{
    return exp( - pow(x, 2));
}
double f5(double x)
{
    if (x <= 0)
    {
        return 0;
    }
    else
    {
        return x;
    }
}
double f6(double x)
{
    int b = 4;
    return (x + sqrt(pow(x, 2) + b))/2;
}

void automaticky_rezim(double xstart, double xstop, double step)
{
    while (xstart <= xstop + 0.00001)
    {
        printf("%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf", xstart, f1(xstart), f2(xstart), f3(xstart), f4(xstart), f5(xstart), f6(xstart));
        if ((xstart + step) < xstop + 0.00001)
        {
            printf("\n");
        }
        xstart += step;
    }
}

void manualny_rezim()
{
    double x;
    while (scanf("%lf", &x) == 1)
    {
        printf("%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf%10.2lf\n", x, f1(x), f2(x), f3(x), f4(x), f5(x), f6(x));
    }
    
}
int main()
{
    int a = 3;
    double xstart, xstop, step;
    scanf("%i", &a);
    if (a == 0)
    {
        if ((scanf("%lf", &xstart) == 1) && (scanf("%lf", &xstop) == 1) &&(scanf("%lf", &step) == 1))
        {
            if (xstart >= xstop)
            {
                printf("E3");
                return 0;
            }

            automaticky_rezim(xstart, xstop, step);
        }
        else
        {
            printf("E2");
        }

    }
    else if (a == 1)
    {
        manualny_rezim();
    }
    else
    {
        printf("E1");
    }
    return 0;
}
