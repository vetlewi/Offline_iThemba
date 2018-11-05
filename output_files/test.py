from numpy import *
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def func_shape(E, a, b, n):
	return a + b*E**n

t = array([91., 82., 74., 72., 71.])
E = array([110, 149, 220, 360, 460])

popt, cov = curve_fit(func_shape, E, t, p0=[73., 1., -2.])

print popt

plt.plot(E, t)
plt.plot(linspace(E[0], E[-1], 1001), func_shape(linspace(E[0], E[-1], 1001), popt[0],popt[1],popt[2]))
plt.show()