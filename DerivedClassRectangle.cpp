/*============================================================================
 Name        : DerivedClassRectangle.cpp
 Author      : Nitin Puranik
 Description : This class derives from the generic base class Shape
 	 	 	   and customizes the class for rectangle objects by implementing
 	 	 	   its own methods.
 ============================================================================*/

#include "BaseClassShape.h"

using namespace std;

class Rectangle : public Shape {
private:
	/* A handy function to find the slope of an edge */
	float Slope (int index);

public:
	Rectangle ( );

	/* Thoroughly check the user input for correctness of data */
	bool SanityCheck ( );
};

/*
 * The default constructor that initializes a rectangle
 * object, accepts user input and does an exhaustive
 * sanity check to see if the user input is error free.
 */
Rectangle::Rectangle ( ) : Shape (4) {

	/*
	 * The English name for the object. Helps
	 * provide user friendly output messages.
	 */
	mName = "Rectangle";

	while (1) {

		int i;
		float point;

		/* Accept user input */
		for (i = 0; i < 8; i++) {

			if (std::cin >> point)
				mPoints[i] = point;

			else {
				std::cout << "Invalid input. Please try again." << std::endl;
				std::cout << std::endl << "Coordinates: ";
				std::cin.clear ( );
				while (std::cin.get ( ) != '\n');
				break;
			}
		}

		if (i == 8) {

			if (SanityCheck() == true)
				break;

			else {
				std::cout << "Ill formed rectangle. Coordinates incorrect or non-sequential."
						<< " Please try again." << std::endl << std::endl;
				std::cout << "Coordinates: ";
				std::cin.clear ( );
				while (std::cin.get ( ) != '\n');
			}
		}
	}

	std::cin.clear ( );
	while (std::cin.get ( ) != '\n');
}

/*
 * A handy method that returns the slope
 * of a given edge in a 2-D plane. The edge
 * is identified by the index, which is the
 * index of one of the x-coordinates of the edge.
 */
float Rectangle::Slope (int index) {
	float x1,y1,x2,y2;

	x1 = mPoints[2 * index];
	x2 = mPoints[(2 * index + 2) % 8];

	/* Parallel to y-axis */
	if (x1 == x2)
		return INV;

	y1 = mPoints[2 * index + 1];
	y2 = mPoints[(2 * index + 3) % 8];

	/* Parallel to x-axis */
	if (y1 == y2)
		return 0;

	/* Angled to the coordinates */
	return (y1 - y2) / (x1 - x2);
}

/*
 * Thoroughly tests the user input to see if the rectangle
 * can be well formed with the provided input points. This
 * function tests if the opposite sides of the rectangle are
 * parallel to each other and if the adjacent sides also are
 * perpendicular to each other.
 *
 */
bool Rectangle::SanityCheck ( ) {

	/* The four slopes for the four edges */
	float slope[4];

	for (int i = 0; i < 4; i++)
		slope[i] = Slope (i);

	/* Tests if opposite sides are parallel */
	if (slope[0] != slope[2]) return false;

	if (slope[1] != slope[3]) return false;

	/* Tests if adjacent sides are perpendicular */
	if (slope[0] == 0 || slope[0] == INV) {
		return slope[0] == 0 ? slope[1] == INV : slope[1] == 0;
	}

	if (slope[1] == 0 || slope[1] == INV) return false;

	return (slope[0] * slope[1] == -1) ? true : false;
}

/*
 * Accepts the rectangular coordinates from the user.
 * Specifies the expected format in which the input
 * is to be provided.
 */
void UserInput ( ) {

	std::cout << std::endl;
	std::cout << "Enter the 8 coordinate vertex points, one pair at a time." << std::endl
			  << "Start from any vertex. Proceed clockwise or anti-clockwise." << std::endl;

	std::cout << std::endl << "Format specified as below (space or newline separated):" << std::endl;
	std::cout << "x1 y1 x2 y2 x3 y3 x4 y4" << std::endl << std::endl;
	std::cout << "Coordinate sequence specified as below (Can be rotated in any desired direction):" << std::endl;
	std::cout << "(x1,y1) ____________ (x2,y2)" << std::endl;
	std::cout << "       |            |       " << std::endl;
	std::cout << "       |            |       " << std::endl;
	std::cout << "(x4,y4)|____________|(x3,y3)" << std::endl << std::endl;
	std::cout << "[Rectangle A] Coordinates: ";

	Rectangle A;

	std::cout << std::endl << "[Rectangle B] Coordinates: ";

	Rectangle B;

	Analyzer (A, B);
}

/*
 * The initial user interface. Provides the user with the
 * menu items using which the application can be navigated.
 */
void WelcomeScreen ( ) {

	unsigned short choice;

	std::cout << " ----------------------------------------------------------"  << std::endl;
	std::cout << "|             Rectangle Features Analyzer                  |" << std::endl;
	std::cout << " ----------------------------------------------------------"  << std::endl;

	while (1) {

		std::cout << std::endl;
		std::cout << "MENU" << std::endl;
		std::cout << "[1] Enter Rectangular Coordinates" << std::endl << "[2] About" << std::endl
				<< "[3] Exit Program" << std::endl << std::endl;
		std::cout << "Enter your choice: ";

		if (std::cin >> choice) {

			switch (choice) {
			case 1:
				UserInput ( );
				break;

			case 2:
				std::cout << std::endl;
				std::cout << "This is a utility application that analyzes 2-D rectangular" << std::endl
						  << "objects to detect their mutual spatial characteristics." << std::endl
						  << "The app supports rectangles that are not only aligned with" << std::endl
						  << "the two axes but are also rotated at an angle to the axes." << std::endl;
				std::cout << std::endl;
				break;

			case 3:
				return;

			default:
				std::cin.clear ( );
				while (std::cin.get ( ) != '\n');
				std::cout << "Invalid choice. Please try again." << std::endl << std::endl;
				break;
			}
		}

		else {
			std::cin.clear();
			while (std::cin.get() != '\n');
			std::cout << "Invalid choice. Please try again." << std::endl << std::endl;
		}
	}
}

int main (int argc, char **argv) {
	WelcomeScreen( );
	return 0;
}
