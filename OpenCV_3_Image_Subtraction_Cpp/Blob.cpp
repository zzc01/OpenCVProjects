#include "Blob.h"

Blob::Blob(std::vector<cv::Point> _contour)
{
	// collect member value 
	contour = _contour; 

	// get bounding rectangle, which is a rect object 
	boundingRect = cv::boundingRect(contour);

	// calcualte the center of the rect object 
	centerPosition.x = (boundingRect.x + boundingRect.x + boundingRect.width) / 2;
	centerPosition.y = (boundingRect.y + boundingRect.y + boundingRect.height) / 2;

	// calcualte the diagonal of the rect 
	dblDiagonalSize = sqrt(pow(boundingRect.width, 2) + pow(boundingRect.height, 2));

	// calcaulte the aspect ratio of the rect 
	// for a normal men this aspect ration < 0.5 
	dblAspectRatio = (double)boundingRect.width / boundingRect.height;
}