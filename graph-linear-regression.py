import numpy as np
import matplotlib.pyplot as plt

# Generate some sample data
np.random.seed(0)
X = np.random.rand(100, 1) * 10  # Random data for X (feature)
y = 2.5 * X + np.random.randn(100, 1) * 2  # Linear relation with some noise

# Adding an intercept term (bias term)
X_b = np.c_[np.ones((X.shape[0], 1)), X]

# Applying the Normal Equation to find the best-fitting parameters (theta)
theta_best = np.linalg.inv(X_b.T.dot(X_b)).dot(X_b.T).dot(y)

# Predicting the values using the model
y_pred = X_b.dot(theta_best)

# Plotting the data points and the regression line
plt.scatter(X, y, color='blue', label='Data points')  # Scatter plot for original data
plt.plot(X, y_pred, color='red', label='Regression Line')  # Line plot for regression line
plt.title('Linear Regression')
plt.xlabel('X')
plt.ylabel('y')
plt.legend()
plt.show()

# Optionally, print the parameters
print(f"Intercept (bias term): {theta_best[0]}")
print(f"Slope: {theta_best[1]}")
