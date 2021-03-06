#include <iostream>
#include <fstream>
#include <string>
struct Interval
{
	float lowerBound;
	float higherBound;
	Interval() {
		lowerBound = 100.0f;
		higherBound = 0.0f;
	};
	Interval(float low, float high) {
		lowerBound = low;
		higherBound = high;
	}
};

struct ModelStats
{
	int TP, TN, FP, FN;
	float accuracy;
	float precision;
	float specificity;
	float recall;
	float FPR;
	ModelStats() {

	}
	ModelStats(int TP, int TN, int FP, int FN, float accuracy, float precision, float specificity, float recall, float FPR) {
		this->TP = TP;
		this->TN = TN;
		this->FP = FP;
		this->FN = FN;
		this->accuracy = accuracy;
		this->precision = precision;
		this->specificity = specificity;
		this->recall = recall;
		this->FPR = FPR;
	}
};


// Compare one instance against the specified model, return prediction
int checkLine(std::string &line, Interval* model)
{
	std::string currentParameter;
	std::string::iterator iter = line.begin();
	for (unsigned int i = 0; i < 57; i++)
	{
		currentParameter = "";
		while (iter != line.end() && *iter != ',')
		{
			currentParameter += *iter;
			iter++;
		}
		float currentParameterFloat = std::stof(currentParameter);
		if (currentParameterFloat != 0)
		{
			if (currentParameterFloat < model[i].lowerBound || currentParameterFloat > model[i].higherBound)
			{
				return 1; // Parameter does not match model, predict spam
			}
		}
		iter++;
	}
	return 0; // Parameters match model, predict ham
}


void trainCross(Interval validationRange, Interval* model)
{
	std::string line;
	std::ifstream myfile("../2788HamShuffled1813Spam.data");
	int currentLine = 0;
	if (myfile.is_open())
	{
		while (getline(myfile, line) && currentLine < 2788) // Early exit once the end of the ham data has been reached
		{
			if (currentLine < validationRange.lowerBound || currentLine > validationRange.higherBound) // Avoid using the validation set to train on
			{

				std::string currentParameter;
				std::string::iterator iter = line.begin();
				for (unsigned int i = 0; i < 57; i++)
				{
					currentParameter = "";
					while (iter != line.end() && *iter != ',')
					{
						currentParameter += *iter;
						iter++;
					}
					float currentParameterFloat = std::stof(currentParameter);
					if (line.back() == '0') // Current training data node is Ham
					{
						if (currentParameterFloat > 0.00001f)
						{
							if (currentParameterFloat < model[i].lowerBound) {
								model[i].lowerBound = currentParameterFloat;
							}
							else if (currentParameterFloat > model[i].higherBound) {
								model[i].higherBound = currentParameterFloat;
							}
						}
					}
					iter++;
				}
			}
			currentLine++;
		}
		//for (unsigned int i = 0; i < 57; i++) {
		//	std::cout << model[i].lowerBound << '|' << model[i].higherBound << std::endl;
		//}
	}
	else
	{
		std::cout << "Unable to open file." << std::endl;
	}
	myfile.close(); // Close the file before exiting

}


ModelStats validateCross(Interval validationRange, Interval* model)
{
	std::string line;
	std::ifstream myfile("../2788HamShuffled1813Spam.data");
	int currentLine = 0;
	int TP = 0, TN = 0, FP = 0, FN = 0;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			// Avoid using the training set for the current model for validation
			if (currentLine >= 2788 || (currentLine >= validationRange.lowerBound && currentLine <= validationRange.higherBound))
			{
				int prediction = checkLine(line, model);
				if (line.back() == '1')
				{
					if (prediction == 1)
					{
						TN++; // Predicted spam, it was spam
					}
					else
					{
						FP++; // Predicted not spam, it was spam
					}
				}
				else
				{
					if (prediction == 1)
					{
						FN++; // Predicted spam, it wasnt spam
					}
					else {
						TP++; // Predicted not spam, it wasnt spam
					}
				}
			}
			currentLine++;
		}

	}
	float precision = (float)TP / (float)(TP + FP);
	float recall = (float)TP / (float)(TP + FN);
	float accuracy = (float)(TP + TN) / (float)(TP + TN + FP + FN);
	float specificity = (float)TN / (float)(TN + FP);
	float FPR = (float)FP / (float)(FP + TN);
	myfile.close(); // Close the file before exiting
	return ModelStats(TP, TN, FP, FN, accuracy, precision, specificity, recall, FPR);
}

#define GROUPSIZE 697
void crossValidation()
{
	std::string line;
	std::ifstream myfile("../2788HamShuffled1813Spam.data");
	// k-fold Cross validating with 17 groups -> group size of 164 (2788 / 17)
	int bestGroup = 0;
	ModelStats bestStats;
	float bestF1 = -1000;
	Interval bestModel[57];
	for (unsigned int i = 0; i < 4; i++)
	{
		std::cout << "Cross validating group #" << i << std::endl;
		Interval model[57];
		Interval validationRange = Interval(i * GROUPSIZE, (i + 1) * GROUPSIZE - 1);
		// Train the model
		trainCross(validationRange, model);
		// Validate the model
		ModelStats currentResult = validateCross(validationRange, model);
		float currentF1 = 2.0f * ((currentResult.precision * currentResult.recall) / (currentResult.precision + currentResult.recall));
		// Optimizing for F1
		if (currentF1 > bestF1)
		{
			bestGroup = i;
			bestStats = currentResult;
			bestF1 = currentF1;
			std::memcpy(bestModel, model, sizeof(Interval) * 57);
		}
	}
	for (unsigned int i = 0; i < 57; i++) {
		std::cout << bestModel[i].lowerBound << '|' << bestModel[i].higherBound << std::endl;
	}
	std::cout << "Best validation group: " << bestGroup << std::endl;
	std::cout << "Group precision: " << bestStats.precision << std::endl;
	std::cout << "Group specificity: " << bestStats.specificity << std::endl;
	std::cout << "Group recall: " << bestStats.recall << std::endl;
	std::cout << "Group accuracy: " << bestStats.accuracy << std::endl;
	std::cout << "Group FPR: " << bestStats.FPR << std::endl;
	std::cout << "Group F1: " << bestF1 << std::endl;
	getchar();
}

int main()
{

	crossValidation();
}



