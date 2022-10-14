#include "ObjectLibrary.h"
#include <fstream>
#include <sstream>
#include <iostream>


ObjectLibrary::ObjectLibrary(std::string libraryRootDirectory, std::string descriptionFilePath, std::string largeObjectProbabilityFile, std::string smallObjectProbabilityFile)
{
	LibraryRoot = libraryRootDirectory;

	std::ifstream descriptionFile(descriptionFilePath);
	std::string line;

	while (std::getline(descriptionFile, line))
	{
		Object object = ReadObject(line);
		if (object.IsLargeObject)
			LargeObjects[object.Name].push_back(object);
		else
			SmallObjects[object.Name].push_back(object);
	}

	if (largeObjectProbabilityFile != "")
		LargeObjectProbability = ReadProbabilityVector(largeObjectProbabilityFile);
	if (smallObjectProbabilityFile != "")
		SmallObjectProbability = ReadProbabilityVector(smallObjectProbabilityFile);

}

std::vector<std::tuple<int, std::string>> ObjectLibrary::ReadProbabilityVector(std::string filePath)
{
	std::ifstream probabilityFile(filePath);
	std::string line;

	std::vector<std::tuple<int, std::string>> result;
	while (std::getline(probabilityFile, line))
	{
		std::istringstream lineStream(line);
		std::string className;
		double probability;
		lineStream >> className >> probability;

		int classProbability = 100.0 * probability;
		if (result.size() == 0)
			result.push_back(std::tuple<int, std::string>(classProbability, className));
		else
			result.push_back(std::tuple<int, std::string>(std::get<0>(result[result.size() - 1]) + classProbability, className));
	}

	return result;
}

Object ObjectLibrary::ReadObject(std::string textDescription)
{
	std::istringstream lineStream(textDescription);

	std::string file, name, wnids;
	double minScale, maxScale;
	bool isLarge;

	// TODO insert error handling
	lineStream >> file >> name >> wnids >> minScale >> maxScale >> isLarge;

	return Object(name, LibraryRoot + file, wnids, minScale, maxScale, isLarge);

}


Object ObjectLibrary::GiveRandomObject(std::default_random_engine &randomEngine, bool largeObject)
{
	if (largeObject)
	{
		return GetRandomObjectFromLibrary(randomEngine, LargeObjects, LargeObjectProbability);
	}
	else
	{
		return GetRandomObjectFromLibrary(randomEngine, SmallObjects, SmallObjectProbability);
	}
}

Object ObjectLibrary::GetRandomObjectFromLibrary(std::default_random_engine& randomEngine, std::map<std::string, std::vector<Object>> objectMap, 
	std::vector<std::tuple<int, std::string>> probabilities)
{
	std::string category = GetRandomCategory(probabilities, randomEngine);
	std::uniform_int_distribution<int> uniformDistribution(0, objectMap[category].size() - 1);
	int objectIndex = uniformDistribution(randomEngine);
	std::cout << category << " " << objectIndex << " " << objectMap[category][objectIndex].AssociatedFile << std::endl;
	return objectMap[category][objectIndex];
}

std::string ObjectLibrary::GetRandomCategory(std::vector<std::tuple<int, std::string>> probabilities, std::default_random_engine& randomEngine)
{
	if (probabilities.size() > 1)
	{
		std::uniform_int_distribution<int> categoryRandomizer(0, std::get<0>(probabilities[probabilities.size() - 1]));
		int categoryNumber = categoryRandomizer(randomEngine);

		int i = 0;
		while (i < probabilities.size() && categoryNumber > std::get<0>(probabilities[i]))
			i++;

		return std::get<1>(probabilities[i]);
	}
	else
		return std::get<1>(probabilities[0]);
}