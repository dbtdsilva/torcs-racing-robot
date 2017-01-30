def drawCurve(points, t):
  if(len(points)==1):
    print("%7.4f %7.4f" % (points[0][0], points[0][1]))
  else:
    newpoints = []
    print("Len %d" % (len(points) - 1))
    for i in range(len(points)-1):
      x = (1-t) * points[i][0] + t * points[i+1][0]
      y = (1-t) * points[i][1] + t * points[i+1][1]
      newpoints += [(x,y)]
    drawCurve(newpoints, t)

drawCurve([(0,0),(2,2),(10,2),(11,2)], 0.1)
