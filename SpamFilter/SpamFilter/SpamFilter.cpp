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




void buildModel()
{
	Interval model[57];
	std::string line;
	std::ifstream myfile("../spambase.data");
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
					if (line.back() == '1') // Current training data node is Ham
					{
						if (model[i].isValid && currentParameterFloat > 0.00001f)
						{
							if (currentParameterFloat < model[i].lowerBound) {
								model[i].lowerBound = currentParameterFloat;
							}
							else if (currentParameterFloat > model[i].higherBound) { // Possibly redundant check
								model[i].higherBound = currentParameterFloat;
							}
						}
					}
					else
					{
						if (currentParameterFloat > model[i].lowerBound || currentParameterFloat < model[i].higherBound)
						{
							model[i].isValid = false;
							model[i].lowerBound = -1;
							model[i].higherBound = -1;
						}
					}
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

int main()
{
	buildModel();
}



