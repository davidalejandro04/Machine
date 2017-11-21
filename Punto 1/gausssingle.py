
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
scaler = StandardScaler()
X=np.genfromtxt('xtrain.txt')
X = scaler.fit_transform(X)
y=np.genfromtxt('ytrain.txt')

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25,random_state=29)


nmax=20

C_range = np.logspace(-5, 3, nmax)
gamma_range = np.logspace(-3, 1, nmax)

Z=[]
for C in C_range:
    for gamma in gamma_range:
        clf=SVC(kernel='rbf',C=C,gamma=gamma,shrinking=True)
        clf.fit(X_train, y_train)
        y_pred=clf.predict(X_test)
        Z.append([C,gamma,accuracy_score(y_test, y_pred)])
        print("C: %lf gamma: %lf Accuracy: %lf"%(C,gamma,accuracy_score(y_test, y_pred)))



Z=np.asarray(Z)
print(Z.shape)
print(Z)

Zn=np.reshape(Z[:,2],(nmax,nmax))
print(Zn)
axX,axY=np.meshgrid(C_range,gamma_range)
plt.imshow(Zn,extent=[C_range[0],C_range[len(C_range)-1],gamma_range[0],gamma_range[len(gamma_range)-1]],interpolation='spline16',aspect='auto',cmap='hot')
plt.colorbar()
print(np.amax(Zn))
plt.ylabel('$\gamma$')
plt.title('Test accuracy')
plt.xlabel('C')
plt.savefig('gauss.png')
plt.show()

