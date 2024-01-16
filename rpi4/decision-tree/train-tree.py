import pandas as pd
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from sklearn.metrics import accuracy_score
import joblib

# Prepare the data
df = pd.read_csv('dataframe.csv')

X = df.drop('identity', axis=1)
y = df['identity']

categorical_cols = ['cam']
numerical_cols = ['cam-confidence', 'galvanic', 'bpm']

encoder = OneHotEncoder()
categorical_encoded = encoder.fit_transform(X[categorical_cols]).toarray()
categorical_encoded = pd.DataFrame(categorical_encoded, columns=encoder.get_feature_names_out(categorical_cols))
X = pd.concat([X[numerical_cols].reset_index(drop=True), categorical_encoded.reset_index(drop=True)], axis=1)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
clf = DecisionTreeClassifier()

# Train the model
clf.fit(X_train, y_train)

# Evaluate the model
y_pred = clf.predict(X_test)

accuracy = accuracy_score(y_test, y_pred)
print(f"Accuracy: {accuracy}")

# Save the model
joblib.dump(clf, 'decision_tree_checkpoint.joblib')

# Save the encoder
joblib.dump(encoder, 'onehot_encoder_checkpoint.joblib')
