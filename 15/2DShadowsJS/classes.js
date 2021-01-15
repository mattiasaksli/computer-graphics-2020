
//Vertex class, this will hold the triangle vertices (positions, colors)
var Vertex = function(position, color) {
	if (position != undefined) {
		this.position = position;
		this.basePosition = position.slice();
	}
	this.color = color;
};

Vertex.prototype = {
	
	//Sometimes you just need to swap two objects
	swapWith: function(v2) {
		var swap = new Vertex().copy(v2);
		v2.copy(this);
		this.copy(swap);
	},
	
	//Or copy an object
	copy: function(v) {
		this.position = v.position.slice();
		this.basePosition = v.basePosition.slice();
		this.color = new Color().copy(v.color);
		
		return this;
	},
	
	//Distance from the current vertex to vertex v
	distance: function(v) {
	
		return Math.sqrt(Math.pow(v.position[0] - this.position[0], 2) + Math.pow(v.position[1] - this.position[1], 2));
	},
	
	//Rotate the current vertex.
	rotate: function(alpha) {
		alpha = alpha / 360 * Math.PI * 2; //Convert angle to radians
		var x = this.basePosition[0] - 100; //Translate our positions so that 0, 0 will be somewhere in the center of the triangle
		var y = this.basePosition[1] - 100;
		this.position[0] = Math.round(x * Math.cos(alpha) - y * Math.sin(alpha)  + 100); //Rotate and translate back
		this.position[1] = Math.round(x * Math.sin(alpha) + y * Math.cos(alpha) + 100);
		
		//Soon we will be looking at transformations in more detail. You may read about them before from CGLearn or:
		//http://en.wikipedia.org/wiki/Rotation_matrix 
	},
	
	//This is the slope of the line from the current vertex to vertex v. 
	//If the line is steep, it is the inversed slope.
	slope: function(v) {
		var deltaX = v.position[0] - this.position[0];
		var deltaY = v.position[1] - this.position[1];
		
		return (this.isSteep(v)) ? deltaX / deltaY : deltaY / deltaX;
	},
	
	//Is the line steep? (|slope| > 1)
	isSteep: function(v) {
		var deltaX = v.position[0] - this.position[0];
		var deltaY = v.position[1] - this.position[1];
		
		return Math.abs(deltaX) < Math.abs(deltaY);
	}
};


//Line class
var Line = function(v0, v1) {
	this.v0 = v0;
	this.v1 = v1;
};

Line.prototype = {

	intersectRay: function(ray) {
	
	}

}

var Ray = function(start, dir) {
	this.start = start;
	this.dir = dir;
}

