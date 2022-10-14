#pragma once
#include <vector>
#include <map>
#include "Object.h"


class ObjectLibrary
{
public:
	ObjectLibrary(std::string libraryRootDirectory, std::string descriptionFilePath, std::string largeObjectProbabilityFile="", std::string smallObjectProbabilityFile="");
	/// <summary>
	/// Read an object library description
	/// </summary>
	/// <param name="textDescription">A line of the library file containing the object informations</param>
	/// <returns>An object which can be used by the library</returns>
	Object ReadObject(std::string textDescription);
	/// <summary>
	/// Give a random object from the library
	/// </summary>
	/// <param name="randomEngine">Random engine to use to draw the random object</param>
	/// <param name="largeObject">If true, an object of a class considered large will be drawn, else the object will be from the "small" classes</param>
	/// <returns>A random object</returns>
	Object GiveRandomObject(std::default_random_engine& randomEngine, bool largeObject);
	/// <summary>
	/// Read a vector of probability, by class, for a category of object (as of now : large and small)
	/// </summary>
	/// <param name="filePath">The path where the file containing the probability vector is stored</param>
	/// <returns>A probability vector : each index correspond to a class, with a probability (in percent) and the class name</returns>
	std::vector<std::tuple<int, std::string>> ReadProbabilityVector(std::string filePath);
	/// <summary>
	/// Draw a random category from a probability vector
	/// </summary>
	/// <param name="probabilities">The probability vector from which to draw a category</param>
	/// <param name="randomEngine">Random engine used for the draw</param>
	/// <returns>The name of the drawn category</returns>
	std::string GetRandomCategory(std::vector<std::tuple<int, std::string>> probabilities, std::default_random_engine& randomEngine);
	/// <summary>
	/// Draw a random object from a specific library
	/// </summary>
	Object GetRandomObjectFromLibrary(std::default_random_engine& randomEngine, std::map<std::string, std::vector<Object>> objectMap, std::vector<std::tuple<int, std::string>> probabilities);

public:
	/// <summary>
	/// Root of the object library, from there the library description file should be able to points towards the object model
	/// </summary>
	std::string LibraryRoot;
	/// <summary>
	/// Every large objects, organised in different list, each indexed by class
	/// </summary>
	std::map<std::string, std::vector<Object>> LargeObjects;
	/// <summary>
	/// Every small objects, organised in different list, each indexed by class
	/// </summary>
	std::map<std::string, std::vector<Object>> SmallObjects;
	/// <summary>
	/// Probability of each class of large object 
	/// </summary>
	std::vector<std::tuple<int, std::string>> LargeObjectProbability;
	/// <summary>
	/// Probability of each class of small object 
	/// </summary>
	std::vector<std::tuple<int, std::string>> SmallObjectProbability;
};

