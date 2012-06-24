#include "preetham.h"

#include <math.h>

static inline double _angleBetween(double thetav, double phiv, double theta, double phi) 
{
    double cospsi = sin(thetav) * sin(theta) * cos(phi-phiv) + cos(thetav) * cos(theta);
    if (cospsi > 1.0) return 0.0;
    if (cospsi < -1.0) return M_PI;
    return acos(cospsi);
}

static inline Color _xyYToRGB(double x, double y, double Y)
{
	double fX, fY, fZ;
        Color result;

	fY = Y;
	fX = x / y * Y;
	fZ = ((1.0 - x - y) / y) * Y;

	double r, g, b;
	r =  3.2404 * fX - 1.5371 * fY - 0.4985 * fZ;
	g = -0.9692 * fX + 1.8759 * fY + 0.0415 * fZ;
	b =  0.0556 * fX - 0.2040 * fY + 1.0573 * fZ;

	double expo = -(1.0 / 10000.0);
	r = 1.0 - exp(expo * r);
	g = 1.0 - exp(expo * g);
	b = 1.0 - exp(expo * b);

	if (r < 0.0) r = 0.0;
	if (g < 0.0) g = 0.0;
	if (b < 0.0) b = 0.0;

	result.r = r;
	result.g = g;
	result.b = b; 
        result.a = 1.0;
        
        colorNormalize(&result);
        
        return result;
}

static inline void _directionToThetaPhi(Vector3 direction, double* theta, double* phi)
{
    direction = v3Normalize(direction);
    *phi = atan2(-direction.z, direction.x);
    *theta = M_PI_2 - asin(direction.y);
}

Color preethamGetSkyColor(Vector3 viewer, Vector3 direction, Vector3 sun_direction, double turbidity)
{
    double theta, phi;
    double thetaSun, phiSun;
    
    _directionToThetaPhi(direction, &theta, &phi);
    _directionToThetaPhi(sun_direction, &thetaSun, &phiSun);
    
    if (theta > M_PI / 2.0)
    {
        theta = M_PI / 2.0;
    }
    double gamma = _angleBetween(theta, phi, thetaSun, phiSun);
    
    double cosTheta;
    if (theta > M_PI / 2.0)
    {
        cosTheta = 0.0000001;
    }
    else
    {
        cosTheta = cos(theta);
    }
    
    double T = turbidity;
    double T2 = T * T;
    double suntheta = thetaSun;
    double suntheta2 = thetaSun * thetaSun;
    double suntheta3 = thetaSun * suntheta2;

    double Ax = -0.01925 * T - 0.25922;
    double Bx = -0.06651 * T + 0.00081; 
    double Cx = -0.00041 * T + 0.21247;
    double Dx = -0.06409 * T - 0.89887;
    double Ex = -0.00325 * T + 0.04517;

    double Ay = -0.01669 * T - 0.26078;
    double By = -0.09495 * T + 0.00921;
    double Cy = -0.00792 * T + 0.21023;
    double Dy = -0.04405 * T - 1.65369;
    double Ey = -0.01092 * T + 0.05291;

    double AY =  0.17872 * T - 1.46303;
    double BY = -0.35540 * T + 0.42749;
    double CY = -0.02266 * T + 5.32505;
    double DY =  0.12064 * T - 2.57705;
    double EY = -0.06696 * T + 0.37027;

    double cosGamma = cos(gamma);
    cosGamma = cosGamma < 0.0 ? 0.0 : cosGamma;
    double cosSTheta = fabs(cos(thetaSun));
    
    double xz = ( 0.00165 * suntheta3 - 0.00375 * suntheta2 + 0.00209 * suntheta + 0.00000) * T2 +
		 (-0.02903 * suntheta3 + 0.06377 * suntheta2 - 0.03202 * suntheta + 0.00394) * T +
		 ( 0.11693 * suntheta3 - 0.21196 * suntheta2 + 0.06052 * suntheta + 0.25886);

    double yz = ( 0.00275 * suntheta3 - 0.00610 * suntheta2 + 0.00317 * suntheta + 0.00000) * T2 +
                (-0.04214 * suntheta3 + 0.08970 * suntheta2 - 0.04153 * suntheta + 0.00516) * T +	
                ( 0.15346 * suntheta3 - 0.26756 * suntheta2 + 0.06670 * suntheta + 0.26688);
    
    double X = (4.0 / 9.0 - T / 120.0) * (M_PI - 2.0 * suntheta);		
    double Yz = ((4.0453 * T - 4.9710) * tan(X) - 0.2155 * T + 2.4192) * 1000.0;

    double val1, val2;

    val1 = (1.0 + Ax * exp(Bx / cosTheta)) * (1.0 + Cx * exp(Dx * gamma) + Ex * sqrt(cosGamma));
    val2 = (1.0 + Ax * exp(Bx)) * (1.0 + Cx * exp(Dx * suntheta) + Ex * sqrt(cosSTheta));
    double x = xz * val1 / val2;

    val1 = (1.0 + Ay * exp(By / cosTheta)) * (1.0 + Cy * exp(Dy * gamma) + Ey * sqrt(cosGamma));
    val2 = (1.0 + Ay * exp(By)) * (1.0 + Cy * exp(Dy * suntheta) + Ey * sqrt(cosSTheta));
    double y = yz * val1 / val2;

    val1 = (1.0 + AY * exp(BY / cosTheta)) * (1.0 + CY * exp(DY * gamma) + EY * sqrt(cosGamma));
    val2 = (1.0 + AY * exp(BY)) * (1.0 + CY * exp(DY * suntheta) + EY * sqrt(cosSTheta));
    double Y = Yz * val1 / val2;

    return _xyYToRGB(x, y, Y);
}

Color preethamApplyToObject(Vector3 viewer, Vector3 object_location, Vector3 sun_direction, double turbidity, Color object_color)
{
    /* TODO Aerial perspective */
    return object_color;
}
