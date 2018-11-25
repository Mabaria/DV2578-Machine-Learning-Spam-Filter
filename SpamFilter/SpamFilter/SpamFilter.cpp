#include <iostream>
#include <fstream>
#include <string>
struct Interval
{
	float lowerBound;
	float higherBound;
	bool isValid;
	Interval() {
		lowerBound = 100.0f;
		higherBound = 0.0f;
		isValid = true;
	};
};




void buildModel(Interval* model)
{
	std::string line;
	std::ifstream myfile("../Ham2200Training.data");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
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
						if (model[i].isValid && currentParameterFloat > 0.00001f)
						{
							if (currentParameterFloat < model[i].lowerBound) {
								model[i].lowerBound = currentParameterFloat;
							}
							else if (currentParameterFloat > model[i].higherBound) { 
								model[i].higherBound = currentParameterFloat;
							}
						}
					}
					//else
					//{
					//	if (currentParameterFloat > model[i].lowerBound || currentParameterFloat < model[i].higherBound)
					//	{
					//		model[i].isValid = false;
					//		model[i].lowerBound = -1;
					//		model[i].higherBound = -1;
					//	}
					//}
					iter++;
			}
			//std::cout << line << std::endl;
		}
		for (unsigned int i = 0; i < 57; i++) {
			std::cout << model[i].lowerBound << '|' << model[i].higherBound << std::endl;
		}
	}
	else
	{
		std::cout << "Unable to open file." << std::endl;
	}
	getchar();
}

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

void validateModel(Interval* model)
{
	std::string line;
	std::ifstream myfile("../2402Validation.data");
	int TP = 0, TN = 0, FP = 0, FN = 0;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
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
	}
	float precision = (float)TP / (float)(TP + FP);
	float recall = (float)TP / (float)(TP + FN);
	std::cout << "True Positives: " << TP << std::endl;
	std::cout << "True Negatives: " << TN << std::endl;
	std::cout << "False Positives: " << FP << std::endl;
	std::cout << "False Negatives: " << FN << std::endl;
	std::cout << "--------------------" << std::endl;
	std::cout << "Precision: " << precision << std::endl;
	std::cout << "Recall: " << recall << std::endl;
	getchar();

}

int main()
{
	Interval model[57];
	buildModel(model);
	validateModel(model);
}



