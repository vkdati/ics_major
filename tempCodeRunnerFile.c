int xsquar = (cops[i].x - currentX)*(cops[i].x - currentX);
    int ysquar = (cops[i].y - currentY)*(cops[i].y - currentY);
    int distance = sqrt(xsquar + ysquar);
    return distance;