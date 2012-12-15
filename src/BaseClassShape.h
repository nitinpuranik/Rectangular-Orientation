/*============================================================================
 Name        : BaseClassShape.h
 Author      : Nitin Puranik
 Description : An extensible class Shape that is capable of handling any
 	 		   convex polygonal object, not just limited to rectangles.
 	 		   This class can be easily extended to support a multitude
 	 		   of polygons. One such derived class provided is for rectangle.
 ============================================================================*/

#include <vector>
#include <string>
#include <iostream>

/* A sentinel value to check extreme cases */
#define INV 0xdeadbeef

/*
 * An enumeration to identify overlap property.
 * adj      -> adjacency, where two objects share
 * 			   an edge and the two objects are on either side
 * 			   of the shared edge.
 * contain  -> One object fully encloses another object.
 * apart	-> The two objects are completely apart.
 * none		-> The two objects intersect.
 */
enum CollisionType { adj, contain, apart, none };

/*
 * The base class shape that defines an inheritable interface
 * and provides many of the feature analyzer functionalities
 * that can be applied to any convex polygon. Derived classes
 * are free to overload / override the functions presented here.
 */
class Shape {
protected:

	/* Number of sides to the given object */
	unsigned int mNumSides;

	/* Candidate edges that may possibly be intersecting */
	std::vector<int> mIsectEdge;

	/* The coordinate points for the vertices of the object */
	float *mPoints;

public:
	/* The english name of the given object */
	std::string mName;

	Shape (unsigned int num) : mNumSides (num) {
		mPoints = new float [2 * mNumSides];
	}

	/*
	 * A utility method that tells if the two points x and y
	 * lie on the given edge whose start point is identified
	 * by index.
	 */
	virtual bool LiesOnEdge (float x, float y, int index) const;

	/* Friend function that analyzes the two objects. */
	friend CollisionType Analyze (Shape&, const Shape&);

	/* Friend function that finds the intersection points */
	friend void FindIntersection (const Shape&, const Shape&);

	virtual ~Shape ( ) {
		delete[] mPoints;
	}
};

/*
 * This function returns a boolean value that tells whether
 * the given points x and y lie on a given edge whose start
 * point is identified by index.
 */
bool Shape::LiesOnEdge (float x, float y, int index) const {

	float x1,y1,x2,y2;

	/* Get the edge's vertices from the index */
	x1 = mPoints[2 * index];
	y1 = mPoints[2 * index + 1];

	x2 = mPoints[(2 * index + 2) % (2 * mNumSides)];
	y2 = mPoints[(2 * index + 3) % (2 * mNumSides)];

	/* Check if (x,y) lies inside (x1,y1) and (x2,y2) */
	return ((x >= x1 && x <= x2) || (x >= x2 && x <= x1)) &&
			((y >= y1 && y <= y2) || (y >= y2 && y <= y1));
}

/*
 * The main workhorse function that analyzes the two objects
 * and determines the type of their overlap in a 2-D plane.
 */
CollisionType Analyze (Shape& A, const Shape& B) {

	/* A counter to determine containment */
	unsigned int contain_ct = 0;

	/* A counter to determine adjacency */
	unsigned int adj_ct;

	/* Implementation of the separating line test */
	for (unsigned int i = 0; i < A.mNumSides; i++) {

		float x1,y1,x2,y2;
		float rot_x, rot_y;
		float sum_A, sum_B;

		/* Get the edge's vertices from the index */
		x1 = A.mPoints[2 * i];
		y1 = A.mPoints[2 * i + 1];

		x2 = A.mPoints[(2 * i + 2) % (2 * A.mNumSides)];
		y2 = A.mPoints[(2 * i + 3) % (2 * A.mNumSides)];

		/* Get the rotated vertices */
		rot_x = y2 - y1;
		rot_y = x1 - x2;

		sum_A = sum_B = 0;

		/*
		 * Find the dot product of each vertex of object A
		 * with respect to its own edge.
		 */
		for (unsigned int j = 0; j < A.mNumSides; j++) {

			float x,y;
			float dotprod;

			x = A.mPoints[2 * j];
			y = A.mPoints[2 * j + 1];

			dotprod = (rot_x * (x - x1)) + (rot_y * (y - y1));
			sum_A += dotprod;
		}

		sum_A = sum_A > 0 ? 1 : -1;
		adj_ct = 0;

		/*
		 * Now find the dot product of each vertex of object B
		 * with respect to the current edge of object A.
		 */
		for (unsigned int j = 0; j < B.mNumSides; j++) {

			float x,y;
			float dotprod;

			x = B.mPoints[2 * j];
			y = B.mPoints[2 * j + 1];

			dotprod = (rot_x * (x - x1)) + (rot_y * (y - y1));

			if (dotprod != 0)
				dotprod > 0 ? sum_B++ : sum_B--;
			
			else if (A.LiesOnEdge(x, y, i) == true)
					adj_ct++;
		}

		/* The two objects are completely apart */
		if (sum_B == -sum_A * B.mNumSides)
			return apart;

		if (adj_ct == 2) {
			float sum_temp;
			sum_temp = sum_B > 0 ? 1 : -1;

			/* The two objects share an edge */
			if (sum_temp == -sum_A)
				return adj;

			A.mIsectEdge.push_back (i);
		}

		else if (sum_B == sum_A * B.mNumSides)
			contain_ct++;

		else
			/* Candidate edges that may possibly be intersecting */
			A.mIsectEdge.push_back (i);
	}

	/* One object completely contains the other object */
	if (contain_ct == B.mNumSides)
		return contain;

	/* The two objects are apart */
	return none;
}

/*
 * The calling function that calls the analyzer twice if needed.
 * The first call analyzes the objects with respect to A's edges.
 * If no conclusion is drawn, then the analyzer works with B's edges.
 * The variable which tells which object contains the other object inside it.
 */
CollisionType ProcessData (Shape& A, Shape& B, int *which) {

	*which = 0;
	CollisionType ret = Analyze (A, B);

	if (ret != none) return ret;

	*which = 1;
	return Analyze (B, A);
}

/*
 * The utility method that finds the points of intersection
 * between the two objects. This function is only called after
 * it has been determined that the two objects intersect.
 */
void FindIntersection (const Shape& A, const Shape&B) {

	/* Iterator that iterators over the indices of candidate edges */
	std::vector<int>::const_iterator A_vit = A.mIsectEdge.begin();

	for (; A_vit != A.mIsectEdge.end(); A_vit++ ) {

		int A_index = *A_vit;
		float A_x1,A_y1,A_x2,A_y2;
		float A_slope, A_intercept;

		/* Points of intersection */
		float intr_x, intr_y;

		/* Get A's edge points */
		A_x1 = A.mPoints[2 * A_index];
		A_y1 = A.mPoints[2 * A_index + 1];

		A_x2 = A.mPoints[(2 * A_index + 2) % (2 * A.mNumSides)];
		A_y2 = A.mPoints[(2 * A_index + 3) % (2 * A.mNumSides)];

		/* Initialize the slope */
		A_slope = INV;

		/* Edge parallel to y-axis */
		if (A_x1 == A_x2)
			intr_x = A_x1;

		/* Edge parallel to x-axis */
		else if (A_y1 == A_y2)
			intr_y = A_y1;

		/* Edge at an angle to the axes */
		else {
			A_slope = (A_y1 - A_y2) / (A_x1 - A_x2);
			A_intercept = A_y1 - A_slope * A_x1;
		}

		/* Work your way through the candidate edges of B */
		std::vector<int>::const_iterator B_vit = B.mIsectEdge.begin();

		for (; B_vit != B.mIsectEdge.end(); B_vit++ ) {

			int B_index = *B_vit;
			float B_x1,B_y1,B_x2,B_y2;
			float B_slope, B_intercept;

			/* Appropriately reset the intersection points */
			if (A_x1 == A_x2)
				intr_y = INV;

			else if (A_y1 == A_y2)
				intr_x = INV;

			else {
				intr_x = INV;
				intr_y = INV;
			}

			/* Get the end point vertices of B's edge */
			B_x1 = B.mPoints[2 * B_index];
			B_y1 = B.mPoints[2 * B_index + 1];

			B_x2 = B.mPoints[(2 * B_index + 2) % (2 * B.mNumSides)];
			B_y2 = B.mPoints[(2 * B_index + 3) % (2 * B.mNumSides)];

			B_slope = INV;

			/*
			 * This condition tests if the two edges are parallel.
			 * This would mean no intersection. Hence we move on.
			 */
			if ((A_x1 == A_x2 && B_x1 == B_x2) ||
					(A_y1 == A_y2 && B_y1 == B_y2))
				continue;

			/* Edge parallel to y-axis */
			if (B_x1 == B_x2)
				intr_x = B_x1;

			/* Edge parallel to x-axis */
			else if (B_y1 == B_y2)
				intr_y = B_y1;

			/* Edge at an angle to the axes */
			else {
				B_slope = (B_y1 - B_y2) / (B_x1 - B_x2);
				B_intercept = B_y1 - B_slope * B_x1;
			}

			/*
			 * This condition tests if the two edge lines intersect
			 * perpendicular to each other.
			 */
			if ((intr_x == A_x1 && intr_y == B_y1) ||
					(intr_x == B_x1 && intr_y == A_y1)) {

				/*
				 * This is an important function that tests
				 * if the actual edges intersect or is it just
				 * the extended lines that intersect.
				 */
				if (A.LiesOnEdge(intr_x, intr_y, A_index) == true &&
						B.LiesOnEdge(intr_x, intr_y, B_index) == true)
					std::cout << "( " << intr_x << ", " << intr_y << " )" << std::endl;

				continue;
			}

			/*
			 * The below else-if conditions are for cases
			 * where one edge is aligned to an axis and the
			 * other edge is at an angle.
			 */
			else if (intr_x == A_x1 && B_slope != INV) {
				intr_y = B_slope * intr_x + B_intercept;
			}

			else if (intr_x == B_x1 && A_slope != INV) {
				intr_y = A_slope * intr_x + A_intercept;
			}

			else if (intr_y == A_y1 && B_slope != INV) {
				intr_x = (A_y1 - B_intercept) / B_slope;
			}

			else if (intr_y == B_y1 && A_slope != INV) {
				intr_x = (B_y1 - A_intercept) / A_slope;
			}

			/* Both the edges intersect at an angle to each other */
			else {
				intr_x = (B_intercept - A_intercept) / (A_slope - B_slope);
				intr_y = A_slope * intr_x + A_intercept;
			}

			/*
			 * This is a subtle edge case to test if the vertex of
			 * one object itself happens to be the point of intersection.
			 * Without this test below, that intersection point would be
			 * printed twice.
			 */
			if ((intr_x == A_x2 && intr_y == A_y2) ||
					(intr_x == B_x2 && intr_y == B_y2))
				continue;

			/*
			 * This is an important function that tests
			 * if the actual edges intersect or is it just
			 * the extended lines that intersect.
			 */
			if (A.LiesOnEdge(intr_x, intr_y, A_index) == true &&
					B.LiesOnEdge(intr_x, intr_y, B_index) == true)
				std::cout << "( " << intr_x << ", " << intr_y << " )" << std::endl;
		}
	}
}

/*
 * This function calls its helper functions that
 * analyze the overlap features of the two objects
 * and return results. This function then displays
 * the results in a user friendly fashion.
 */
void Analyzer (Shape& A, Shape& B) {

	/*
	 * This variable is to determine which object
	 * completely encloses the other object.
	 */
	int AorB;

	/*
	 * Determine the type of overlap and display
	 * the corresponding message.
	 */
	switch (ProcessData (A, B, &AorB)) {
	case apart:
		std::cout << std::endl << A.mName << " A and "
		<< B.mName << " B are well separated." << std::endl;
		break;

	case adj:
		std::cout << std::endl << A.mName << " A and "
		<< B.mName << " B are adjacent." << std::endl;
		break;

	case contain:
		if (AorB == 0)
			std::cout << std::endl << B.mName << " B is wholly contained "
			<< "within " << A.mName << " A." << std::endl;
		else
			std::cout << std::endl << B.mName << " A is wholly contained "
			<< "within " << A.mName << " B." << std::endl;
		break;

	case none:
		std::cout << std::endl << A.mName << " A and " << B.mName << " B"
		<< " intersect. The points of"
		<< " intersection are: " << std::endl;

		/* The two objects intersect. Find the intersection points */
		FindIntersection (A, B);
		break;
	}
}
