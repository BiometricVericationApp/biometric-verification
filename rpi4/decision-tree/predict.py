import pandas as pd
import joblib

class DecisionTreeClassifier:
    # TODO: Change the model_filename to the absolute path to make the service work
    def __init__(self, model_filename='decision_tree_checkpoint.joblib'):
        self.model_filename = model_filename
        self.encoder = None
        self.loaded_model = None

    def load_model(self):
        # Load the trained model
        self.loaded_model = joblib.load(self.model_filename)
        # Load the encoder from the training phase
        # TODO: Change the path to the absolute path to make the service work
        self.encoder = joblib.load('onehot_encoder_checkpoint.joblib')

    def predict(self, input_str):
        # Convert the input string to a DataFrame
        input_data = pd.DataFrame([input_str.split(',')], columns=['cam', 'cam-confidence', 'galvanic', 'bpm'])

        # If the encoder is not loaded, load it
        if self.encoder is None:
            self.load_model()

        # Prepare the input data
        categorical_encoded = self.encoder.transform(input_data[['cam']]).toarray()
        categorical_encoded = pd.DataFrame(categorical_encoded, columns=self.encoder.get_feature_names_out(['cam']))
        input_data = pd.concat([input_data[['cam-confidence', 'galvanic', 'bpm']].reset_index(drop=True),
                               categorical_encoded.reset_index(drop=True)], axis=1)

        # Make a prediction using the loaded model
        prediction = self.loaded_model.predict(input_data)

        return prediction
