from predict import DecisionTreeClassifier

classifier = DecisionTreeClassifier()
sample = "Ferran,0.7781640927400806,0.795500584366006,125"
prediction = classifier.predict(sample)
print(prediction)
