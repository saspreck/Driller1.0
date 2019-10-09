// Project1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <array>
#include <algorithm>
#include "ResizableArray.h"
#include "Sorter.h"
#include "Search.h"
#include "DrillingRecord.h"
#include "DrillingRecordComparator.h"
#include "Comparator.h"

using namespace std;


/*===================================================================================================================
Function to check if the date stamp matches the correct one
*/
int checkDateStamp(string dataLine, string dateStamp, int lineNum) {
	
	//creates a substring that contains only the date
	string dateCheck = dataLine.substr(0, dataLine.find(','));
	//checks if the substring doesn't match the correct one
	if (dateCheck != dateStamp) {
		//prints out the error message
		std::cout << "Non-matching date stamp " << dateCheck << " at line " << lineNum << "." << endl;
		return false;
	}
	else {
		return true;
	}
	
}
//=========================================================================================================


/*=========================================================================================================
Function to check if the time stamp is unique
*/
bool checkTimeStamp(string dataLine, ResizableArray<DrillingRecord>* dataArray, int lineNum) {

	//makes a substring of everything except the date
	string dataLineMinusDate = dataLine.substr(dataLine.find(',') + 1);

	//makes a substring of the time stamp
	string timeStampToCheck = dataLineMinusDate.substr(0, dataLineMinusDate.find(','));

	//iterates through the given array to check for matching time stamps
	for (unsigned  int i = 0; i < dataArray->getSize(); i++) {

		//checks if time stamp equals one from array and prints error message if so
		if (timeStampToCheck == dataArray->get(i).getString(1)) {
			std::cout << "Duplicate timestamp " << timeStampToCheck << " at line " << lineNum << "." << endl;
			return false;
		}
	}
	return true;

}
//====================================================================================================================


/*====================================================================================================================
Function to check that all the float values are positive
*/
bool checkFloatValues(string dataLine, int lineNum) {
	//declare variables
	string tempData;
	double doubleData;

	//turn input string into a stringstream
	stringstream stringData(dataLine);

	//ignores the date and time stamps
	stringData.ignore(256, ',');
	stringData.ignore(256, ',');

	//iterates through each data point checking
	for (int i = 0; i < 16; i++) {
		//splits each piece of data off from the stringstream and turns it into a double
		getline(stringData, tempData, ',');
		doubleData = stod(tempData);

		//checks if the data is positive and throws an error otherwise
		if (doubleData <= 0) {
			std::cout << "Invalid floating-point data at line " << lineNum << "." << endl;
			return false;
		}
	}
	//returns true if all the data values are positive
	return true;
}
//=====================================================================================================================


/*=====================================================================================================================
Method to output the data based on how the user has selected
*/
void dataOutput(ResizableArray<DrillingRecord>* mainRecordArray, unsigned long RecordsStored, unsigned long totalLinesRead, unsigned long validLinesread) {

	//priming read for the loop to get the output file name
	string outputFileName = "";
	std::cout << "Enter output file name: "; \
	getline(cin, outputFileName, '\n');
	ofstream outputToFile;
	bool printed = false;

	//loops until the data has been printed
	while (printed == false) {

		//checks if the user entered anything
		if (outputFileName.empty()) {

			//iterates through and prints out data
			for (unsigned long i = 0; i < RecordsStored; i++) {

				//try/catch block to catch an IndexOutOfRange exception
				try {
					std::cout << mainRecordArray->get(i) << endl;
					
				}
				catch (ExceptionIndexOutOfRange* e) {
					delete e;
				}
			}
			//prints out different numbers of lines read in
			cout << "Data lines read: " << totalLinesRead << "; Valid drilling records read: " << validLinesread << "; Drilling records in memory: " << RecordsStored << endl;
			printed = true;
		}
		else {
			//attempts to open file and if cannot displays warning
			outputToFile.open(outputFileName);
			if (!outputToFile.is_open()) {
				std::cout << "File is not available" << endl;
				std::cout << "Enter output file name: ";
				getline(cin, outputFileName, '\n');
			}
			//if file opens prints to file
			else {
				//loops through and prints out each record to the file
				
				for (unsigned long i = 0; i < RecordsStored; i++) {
					try {
						outputToFile << mainRecordArray->get(i) << endl;
					}
					catch (ExceptionIndexOutOfRange* e) {
						delete e;
					}
				}
				outputToFile << "Data lines read: " << totalLinesRead << "; Valid drilling records read: " << validLinesread << "; Drilling records in memory: " << RecordsStored << endl;
				printed = true;
			}
			outputToFile.close();
		}
	}
}
//=================================================================================================================

//=================================================================================================================
void searchMethod(ResizableArray<DrillingRecord>* mainRecordArray, unsigned int sortColumn) {

	//asks the user which field to sort on and converts it to an int
	string searchField = "";
	std::cout << "Enter search field (0-17): " << endl;
	getline(cin, searchField, '\n');
	unsigned int column = (unsigned)stoi(searchField);

	//makes sure the field number is valid
	if (column <= 17) {

		//checks to see if a numeric column
		if (column > 1) {

			//asks the user for the value to search for
			string fieldValue = "";
			std::cout << "Enter positive field value: " << endl;
			getline(cin, fieldValue);

			//checks if the user entered something
			if (fieldValue != "") {
				double keyValue = stod(fieldValue);

				//makes sure key is valid and then searches
				if (keyValue > 0) {

					//creates new drilling record and sets the value entered to the correct column
					DrillingRecord* searchRecord = new DrillingRecord;
					try {
						searchRecord->setNum(keyValue, column - 2);
					}
					catch (ExceptionIndexOutOfRange* e) {
						delete e;
					}


					//creates comparator for the search
					Comparator<DrillingRecord>* searchComp = new DrillingRecordComparator(column);
					long long index;
					long long location = 0;

					//checks if the column entered is the same column as the column
					//sorted on and calls correct search
					if (sortColumn == column) {
						index = binarySearch(*searchRecord, *mainRecordArray, *searchComp);
					}
					else {
						index = linearSearch(*searchRecord, *mainRecordArray, *searchComp);
					}


					location = index;
					int numMatches = 0;

					//if the search key is in the array then it iterates through, finding all records with a matching value
					if (index >= 0) {
						while ((unsigned)index < mainRecordArray->getSize()) {

							if (searchComp->compare(mainRecordArray->get(index), mainRecordArray->get(location)) == 0) {
								try {
									cout << mainRecordArray->get(index) << endl;
									numMatches++;
								}
								catch (ExceptionIndexOutOfRange* e) {
									delete e;
								}
							}

							index++;
						}
						//prints out total number of matching records found
						cout << "Drilling records found: " << numMatches << "." << endl;
					}
					else {
						cout << "Drilling records found: 0." << endl;
					}

					//deletes objects
					delete searchRecord;
					searchRecord = nullptr;
				}
			}
		}

		else {
			//asks user what text they would like to find
			string fieldValue = "";
			std::cout << "Enter exact text on which to search:" << endl;
			getline(cin, fieldValue, '\n');

			//checks if user entered something and then searches for the text entered
			if (fieldValue != "") {

				//creates new drilling record and sets the value entered to the correct column
				DrillingRecord* searchRecord = new DrillingRecord;
				try {
					searchRecord->setString(fieldValue, column);
				}
				catch (ExceptionIndexOutOfRange* e) {
					delete e;
				}

				//creates comparator for the search
				Comparator<DrillingRecord>* searchComp = new DrillingRecordComparator(column);
				long long index;
				long long location;

				//checks if the column entered is the same column as the column
				//sorted on and calls correct search
				if (sortColumn == column) {
					index = binarySearch(*searchRecord, *mainRecordArray, *searchComp);
				}
				else {
					index = linearSearch(*searchRecord, *mainRecordArray, *searchComp);
				}

				location = index;
				int numMatches = 0;
				//iterates through array if search returns a nonnegative printing out matches
				if (index >= 0) {
					while ((unsigned)index < mainRecordArray->getSize()) {

						if (searchComp->compare(mainRecordArray->get(index), mainRecordArray->get(location)) == 0) {
							try {
								cout << mainRecordArray->get(index) << endl;
								numMatches++;
							}
							catch (ExceptionIndexOutOfRange* e) {
								delete e;
							}
						}

						index++;
					}
					//prints out total number of matches found
					cout << "Drilling records found: " << numMatches << "." << endl;
				}
				else {
					cout << "Drilling records found: 0." << endl;
				}

				//deletes objects
				delete searchRecord;
				searchRecord = nullptr;

			}
		}
	}
}


//=================================================================================================================
int main() {
	//declare variables
	string userInput = "";
	string initialDateStamp = "";
	ifstream fileName;
	unsigned long totalLinesRead = 0;
	unsigned long validLinesRead = 0;
	unsigned long RecordsCurrentlyStored = 0;

	//declares DrillingRecordArray object to use
	ResizableArray<DrillingRecord>* mainRecordArray = new ResizableArray<DrillingRecord>;
	

	//priming read for the loop, asks the user for the name of the data file and stores it
	std::cout << "Enter data file name: ";
	getline(cin, userInput, '\n');

	/*
	Data input loop, gets data file from user and checks to make sure it is valid
	*/
	while (!userInput.empty()) {

		//variable to keep track of current file line being used
		int fileLineNum = 1;

		//attempts to open the file and tells the user if that fails
		fileName.open(userInput, ifstream::in);
		if (!fileName.is_open()) {
			std::cout << "File is not available." << endl;

			userInput = "";
			std::cout << "Enter data file name: " ;
			getline(cin, userInput, '\n');
			continue;
		}


		//declares variable to store file line in and then skips the first line
		string line = "";
		fileName.ignore(10000, '\n');

		//temporary array to store the drilling records from each file to be merged with full array
		ResizableArray<DrillingRecord>* tempArray = new ResizableArray<DrillingRecord>;

		//boolean to keep track of if the first date in the file has been checked
		bool firstDateChecked = false;

		//while loop to iterate through the file collecting data line by line
		while (getline(fileName, line)) {

			//assign date stamp
			if (initialDateStamp == "") {
				initialDateStamp = line.substr(0, line.find(','));
			}

			//checks first date in the file to make sure it is consistent
			//if not then the file is closed
			if (firstDateChecked == false) {
				string firstDateStamp = line.substr(0, line.find(','));
				if (firstDateStamp != initialDateStamp) {
					fileName.close();
					std::cout << "Date mismatch, file closed. " << endl;
					break;
				}
				firstDateChecked = true;
			}
			

			//calls the checkDateStamp function to ensure date stamps are correct
			if (checkDateStamp(line, initialDateStamp, fileLineNum)) {
				
				//calls the checkFloatValues function to ensure they are positive are unique
				if (checkTimeStamp(line, tempArray, fileLineNum)) {

					//calls the checkFloatValues method to ensure they are positive
					if (checkFloatValues(line, fileLineNum)) {

						//declares a drilling record to use and a string stream
						DrillingRecord* currentRecord = new DrillingRecord;

						//pulls off datestamp and puts it in correct array
						string dateStamp = line.substr(0, line.find(','));
						currentRecord->addString(dateStamp);

						//pulls off timestamp and puts it in correct array
						string noDateStamp = line.substr(line.find(',') + 1);
						string timeStamp = noDateStamp.substr(0, noDateStamp.find(','));
						currentRecord->addString(timeStamp);

						string noTimeStamp = noDateStamp.substr(noDateStamp.find(',') + 1);
						stringstream noStrings(noTimeStamp);

						//splices off each piece of data, turns it to a double, and then puts it into the correct array
						for (int i = 0; i < 16; i++) {
							string tempData = "";
							getline(noStrings, tempData, ',');
							double doubleData = stod(tempData);
							currentRecord->addNum(doubleData);

						}

						//adds the current drilling record to the drilling array and increments the number of valid lines
						tempArray->add(*currentRecord);
						validLinesRead++;
						delete currentRecord;
						currentRecord = nullptr;
						
					}
				}
			}
			//increases the line number
			fileLineNum++;
			totalLinesRead++;
		}
		//after reading all lines from the current file merges the two arrays, replacing items in the main array
		//if they have the same time stamp as an item in the new file
		Comparator<DrillingRecord>* timeComp = new DrillingRecordComparator(1);
		for (unsigned long i = 0; i < tempArray->getSize(); i++) {
			try {

				if (mainRecordArray->getSize() > 0) {

					long long index = linearSearch(tempArray->get(i), *mainRecordArray, *timeComp);

					if (index < 0) {
						mainRecordArray->add(tempArray->get(i));
						
					}
					else {
						mainRecordArray->replaceAt(tempArray->get(i), (unsigned)index);
					}
				}
				else {
					mainRecordArray->add(tempArray->get(i));
				}
			}
			catch(ExceptionIndexOutOfRange* e){
				delete e;
			}
		}
		
		//updates the value of the recordscurrentlystored variable
		RecordsCurrentlyStored = mainRecordArray->getSize();

		//closes filestream and delete objects
		fileName.close();
		delete tempArray;
		tempArray = nullptr;

		//resets userInput and asks user for new filename
		userInput = "";
		std::cout << "Enter data file name: ";
		getline(std::cin, userInput, '\n');

	}
	
	//exits program if nothing was added to array
	if (mainRecordArray->getSize() == 0) {
		//cout << "Thanks for using Driller." << endl;
		return 0;
	}
	
	//sorts array based on time
	DrillingRecordComparator sortComp(1);
	Sorter<DrillingRecord>::sort(*mainRecordArray, sortComp);

	//priming read for the data manipulation loop
	std::cout << "Enter (o)utput, (s)ort, (f)ind, or (q)uit: " << endl;
	getline(cin, userInput, '\n');

	//variable to keep track of which column the data is sorted on, 
	//defaults to time stamps since they are already sorted
	unsigned int sortColumn = 1;

	/*
	Data manipulation loop, asks users whether they would like output, sort, find, or quit
	*/
	while (userInput != "q" && userInput != "Q") {

		//method call for if user enters an o
		if (userInput == "o" || userInput == "O") {
			dataOutput(mainRecordArray, RecordsCurrentlyStored, totalLinesRead, validLinesRead);
		}

		//code for if user enters an s
		if (userInput == "s" || userInput == "S") {

			//asks the user which field to sort on and converts it to an int
			string sortField = "";
			std::cout << "Enter sort field (0-17): " << endl;
			getline(cin, sortField, '\n');
			unsigned int column = (unsigned) stoi(sortField);

			//makes sure it is valid and if so sorts based on that column
			if (column <= 17) {
				sortColumn = column;
				DrillingRecordComparator sortComp(column);
				Sorter<DrillingRecord>::sort(*mainRecordArray, sortComp);
			}
			else {
				continue;
			}
		}

		//code for if user enters an f
		if (userInput == "f" || userInput == "F") {
			searchMethod(mainRecordArray, sortColumn);
		}
		//repeats prompt for action
		std::cout << "Enter (o)utput, (s)ort, (f)ind, or (q)uit: " << endl;
		getline(cin, userInput, '\n');
	}

	delete mainRecordArray;
	mainRecordArray = nullptr;

	std::cout << "Thanks for using Driller." << endl;
}
//=============================================================================================================

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
