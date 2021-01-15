var Objects = {

	createBox: function() {
		//One quad around the entire scene.
		var geometry, material, mesh;
		var halfSize = size / 2;
		var halfPx = 0.5 / halfSize;
		
		geometry = new THREE.Geometry();
		geometry.vertices.push(
			new THREE.Vector3( -halfSize + halfPx	, -halfSize + halfPx, 0 ),
			new THREE.Vector3(  halfSize			, -halfSize + halfPx, 0 ),
			new THREE.Vector3(  halfSize			,  halfSize 		, 0 ),
			new THREE.Vector3( -halfSize + halfPx	,  halfSize			, 0 ),
			new THREE.Vector3( -halfSize + halfPx	, -halfSize + halfPx, 0 )
		);
		material = new THREE.LineBasicMaterial( { color: 0x000000 } );
		mesh = new THREE.Line( geometry, material );
		mesh.name = "box";
		mesh.position.set(0, 0, 2);
        mesh.updateMatrixWorld();
		
		return mesh;
	},
	
	createC: function() {
		
		//Letter C
		geometry = new THREE.Geometry();
		geometry.vertices.push(
			new THREE.Vector3(  10, 10, 0 ),
			new THREE.Vector3(   5, 15, 0 ),
			new THREE.Vector3( -10, 15, 0 ),
			new THREE.Vector3( -20,  0, 0 ),
			new THREE.Vector3( -10,-15, 0 ),
			new THREE.Vector3(   5,-15, 0 ),
			new THREE.Vector3(  10,-10, 0 ),
			new THREE.Vector3(   5,-12, 0 ),
			new THREE.Vector3(  -5,-10, 0 ),
			new THREE.Vector3( -12,  0, 0 ),
			new THREE.Vector3(  -5, 10, 0 ),
			new THREE.Vector3(   5, 12, 0 ),
			new THREE.Vector3(  10, 10, 0 )
		);
		//geometry.applyMatrix4(new THREE.Matrix4().makeScale(5, 5, 1)); //We need to bake the scale inside the geometry due to this bug: https://github.com/mrdoob/three.js/issues/5827
		
		material = new THREE.LineBasicMaterial( { color: 0xff0000, visible: isDebug } );
		mesh = new THREE.Line( geometry, material );
		mesh.name = "letterC";
		mesh.position.set(-70, 0, 2);
		mesh.scale.set(5, 5, 1);
        mesh.updateMatrixWorld();
		
		return mesh;
	},

	createG: function() {
		var geometry, material, mesh;
		
		//Letter G
		geometry = new THREE.Geometry();
		geometry.vertices.push(
			new THREE.Vector3(  10, 10, 0 ),
			new THREE.Vector3(   5, 15, 0 ),
			new THREE.Vector3( -10, 15, 0 ),
			new THREE.Vector3( -20,  0, 0 ),
			new THREE.Vector3( -10,-15, 0 ),
			new THREE.Vector3(   5,-15, 0 ),
			new THREE.Vector3(  10,-10, 0 ),
			new THREE.Vector3(  10, -5, 0 ),
			new THREE.Vector3(   5,  2, 0 ),
			new THREE.Vector3(  -5, -3, 0 ),
			new THREE.Vector3(   3, -2, 0 ),
			new THREE.Vector3(   6, -7, 0 ),
			new THREE.Vector3(  -5,-10, 0 ),
			new THREE.Vector3( -12,  0, 0 ),
			new THREE.Vector3(  -5, 10, 0 ),
			new THREE.Vector3(   5, 12, 0 ),
			new THREE.Vector3(  10, 10, 0 )
		);
		//geometry.applyMatrix4(new THREE.Matrix4().makeScale(5, 5, 1));  //Issue: https://github.com/mrdoob/three.js/issues/5827
		
		material = new THREE.LineBasicMaterial( { color: 0xff0000, visible: isDebug } );
		mesh = new THREE.Line( geometry, material );
		mesh.name = "letterG";
		mesh.position.set(100, 0, 2);
		mesh.scale.set(5, 5, 1);
		mesh.updateMatrixWorld();
		
		return mesh;
	},
		
	createTriangles: function() {
		var triangles = [];
		var geometry, material, mesh;
		//Equilateral triangles
		//Some "random" positions for the triangles.
		var trianglePositions = [
			new THREE.Vector2(-120, 120),
			new THREE.Vector2(-10, 170),
			new THREE.Vector2( 30, 90),
			new THREE.Vector2(140, 110),
			new THREE.Vector2(-140, -140),
			new THREE.Vector2( -50, -170),
			new THREE.Vector2( 60,  -120),
			new THREE.Vector2(140, -110),
		];
		
		//Hopefully you did something like this in the tasks on the very first week ;)
		var sin30 = Math.sin(toRad(30));
		var cos30 = Math.cos(toRad(30));
		var appliedTranslationMatrix;
		var pos = 0;
		for (var pos in trianglePositions) {
			var geometry = new THREE.Geometry();
			geometry.vertices.push(
				new THREE.Vector3(   0, 10, 0 ),
				new THREE.Vector3(  -cos30 * 10, -sin30 * 10, 0 ),
				new THREE.Vector3(  cos30 * 10, -sin30 * 10, 0 ),
				new THREE.Vector3(  0, 10, 0 )
			);
			//geometry.applyMatrix4(new THREE.Matrix4().makeScale(2, 2, 1)); //Issue: https://github.com/mrdoob/three.js/issues/5827
							
			material = new THREE.LineBasicMaterial( { color: 0xff0000, visible: isDebug } );
			var mesh = new THREE.Line( geometry, material );
			mesh.rotationSpeed = (Math.random() > 0.5 ? -1 : 1) * (Math.random() * 2.0 + 1.0);
			mesh.name = "triangle-" + pos;
			mesh.scale.set(2, 2, 1);
			mesh.position.set(trianglePositions[pos].x, trianglePositions[pos].y, 2);
			mesh.updateMatrixWorld();
			
			triangles.push(mesh);
		}
		
		return triangles;
	},
	
	createFloor: function() {
		
		var loader = new THREE.TextureLoader();
		loader.setCrossOrigin('Anonymous');
		
		geometry = new THREE.PlaneGeometry(width, height, 10, 10);
		/**
		 * --Task--
		 * Make this material use multiplicative blending to show the floor only where there is a shadowOverlay
		 * I guess it is the shadow underlay not overlay... :)
		 */
		material = new THREE.MeshBasicMaterial({
			color: "#ffffff",
			map: loader.load('http://cglearn.eu/files/course/7/textures/wallTexture2.jpg'),
			transparent: true,
			blending: THREE.MultiplyBlending,
			blendEquation: THREE.AddEquation,
			blendSrc: THREE.ZeroFactor,	// floor texture
			blendDst: THREE.OneFactor	// shadow underlay
		});
		mesh = new THREE.Mesh(geometry, material);
		mesh.position.z = 1.0;
		
		return mesh;
	}

}