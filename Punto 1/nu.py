
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


scaler = StandardScaler()
X=np.genfromtxt('xtrain.txt')
X = scaler.fit_transform(X)
y=np.genfromtxt('ytrain.txt')

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25,random_state=29)




C=350.0

for i in range(0,400):
    gamma=0.0010+0.00005*i
    clf=SVC(kernel='rbf',C=C,gamma=gamma,shrinking=True)
    clf.fit(X_train, y_train)
    y_pred=clf.predict(X_test)
    print("C: %lf gamma: %lf Accuracy: %lf"%(C,gamma,accuracy_score(y_test, y_pred)))
