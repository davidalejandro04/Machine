
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import Normalize

from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import load_iris
from sklearn.model_selection import StratifiedShuffleSplit
from sklearn.model_selection import GridSearchCV
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score



# Utility function to move the midpoint of a colormap to be around
# the values of interest.

class MidpointNormalize(Normalize):

    def __init__(self, vmin=None, vmax=None, midpoint=None, clip=False):
        self.midpoint = midpoint
        Normalize.__init__(self, vmin, vmax, clip)

    def __call__(self, value, clip=None):
        x, y = [self.vmin, self.midpoint, self.vmax], [0, 0.5, 1]
        return np.ma.masked_array(np.interp(value, x, y))



################################################################################################################
####################Carga de datos
############################################################################################################

X=np.genfromtxt('xtrain.txt')
y=np.genfromtxt('ytrain.txt')
X_prime,y_prime=X,y
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.20,random_state=42)

scaler = StandardScaler()
X = scaler.fit_transform(X)

C_range = np.logspace(-5, 1, 30)
gamma_range = np.logspace(-2, 2, 30)
degree_range = np.linspace(3,10,1)

param_grid = dict(gamma=gamma_range, C=C_range,degree=degree_range)
cv = StratifiedShuffleSplit(n_splits=12, test_size=0.3, random_state=42)
grid = GridSearchCV(SVC(kernel='poly'), param_grid=param_grid, cv=cv)
grid.fit(X_train, y_train)

print("The best parameters are %s with a score of %0.2f"
      % (grid.best_params_, grid.best_score_))

print(grid.cv_results_)

################################################################################################################
####################Escalizacion de datos
############################################################################################################

############################################################################################################



"""
scores=[]

for C in C_range:
    for gamma in gamma_range:
        clf = SVC(C=C, cache_size=200, class_weight=None, coef0=0.0,
            decision_function_shape='ovr', degree=3, gamma=gamma, kernel='poly',
            max_iter=-1, probability=False, random_state=None, shrinking=True,
            tol=0.001, verbose=False)
        clf.fit(X, y)
        y_pred=clf.predict(X_test)
        print("C: %lf gamma: %lf Accuracy: %lf"%(C,gamma,accuracy_score(y_test, y_pred)))
        scores.append([C,gamma,accuracy_score(y_test, y_pred)])
"""

