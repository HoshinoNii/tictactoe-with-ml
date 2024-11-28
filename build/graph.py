# Description: This script reads the metrics from the CSV file
# and plots the confusion matrix as a heatmap and the derived
# metrics as a bar chart. The csv file is generated when the game
# is started, and can be subsequently used to generate the graphs.

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load metrics from the CSV file
data = pd.read_csv("metrics.csv")

# Extract confusion matrix values
confusion_values = [
    int(data.loc[data['Metric'] == "True Positive", 'Value'].values[0]),
    int(data.loc[data['Metric'] == "True Negative", 'Value'].values[0]),
    int(data.loc[data['Metric'] == "False Positive", 'Value'].values[0]),
    int(data.loc[data['Metric'] == "False Negative", 'Value'].values[0])
]

# Extract derived metrics
metrics = data[data['Metric'].str.contains("Precision|Recall|F1-Score")]
metric_values = metrics['Value'].values
metric_labels = metrics['Metric'].values

# Plot confusion matrix as a heatmap
plt.figure(figsize=(6, 5))
sns.heatmap(
    [[confusion_values[0], confusion_values[2]],  # TP, FP
     [confusion_values[3], confusion_values[1]]], # FN, TN
    annot=True, fmt='d', cmap='Blues',
    xticklabels=["Predicted Positive", "Predicted Negative"],
    yticklabels=["Actual Positive", "Actual Negative"]
)
plt.title("Confusion Matrix")
plt.xlabel("Predicted")
plt.ylabel("Actual")
plt.show()
