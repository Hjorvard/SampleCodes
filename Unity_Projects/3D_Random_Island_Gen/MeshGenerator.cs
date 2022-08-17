using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class MeshGenerator 
{

	public static MeshData GenerateTerrainMesh(float[,] heightMap, float heightMultiplier, AnimationCurve heightCurve, int levelOfDetail) 
	{
		//ints
		//get the width and height of the heightmap
		int width = heightMap.GetLength (0);
		int height = heightMap.GetLength (1);
	
		

		//floats
		//find the position of the top left point on the x axis
		float topLeftX = (width - 1) / -2f;
		//find the position of the top left point of the z axis
		float topLeftZ = (height - 1) / 2f;

		//change the level of detail of the mesh
		int meshSimplificationIncrement = (levelOfDetail == 0)?1:levelOfDetail * 2;
		int verticesPerLine = (width - 1) / meshSimplificationIncrement + 1;

		//reference to the MeshData function
		// Takes in the width and height of the map 
		MeshData meshData = new MeshData (verticesPerLine, verticesPerLine);
		//to keep track of where the vertex index is currently at
		int vertexIndex = 0;

		//use a for loop to go through each of the squares on the mesh of the heightmap
		//change this depending on the level of detail of the mesh
		for (int y = 0; y < height; y += meshSimplificationIncrement) 
		{
			for (int x = 0; x < width; x += meshSimplificationIncrement) 
			{

				//0-1-2   0 = top left and 2 triangles, 1 = middle left and 2 triangles, 2 =  no start point as the top left of any triangles so skip onto 3
				//|\|\|   3 = middle left and 2 triangles (going downwards)
				//3-4-5
				//|\|\|	  vertex = width * height
				//6-7-8	  triangles = (width - 1( * (height - 1) * 6 
				//
				//   i------i+
				//   |       |
				//  i+w-----i+w+1
				//
				//so for example a triangle starting at the top left would be i, i + w + 1, i + 1, going clockwise
				meshData.vertices [vertexIndex] = new Vector3 (topLeftX + x, heightCurve.Evaluate(heightMap [x, y]) * heightMultiplier, topLeftZ - y);
				//tell each vertext its position as a percentage between 0 and 1
				meshData.uvs [vertexIndex] = new Vector2 (x / (float)width, y / (float)height);

				if (x < width - 1 && y < height - 1) 
				{
					//triangle that starts at the top left and moves to the bottom right then bottom left
					meshData.AddTriangle (vertexIndex, vertexIndex + verticesPerLine + 1, vertexIndex + verticesPerLine);
					//triangle that starts at the bottom right, moves to the top left, then top right 
					meshData.AddTriangle (vertexIndex + verticesPerLine + 1, vertexIndex, vertexIndex + 1);
				}

				//add 1 to the vertex index so that we can keep track of how many there are
				vertexIndex++;
			}
		}
		//need to return the meshData
		return meshData;

	}
}

//mesh data function
public class MeshData 
{
	//array of 3D vectors that represent the points of the vertices on the mesh
	public Vector3[] vertices;
	//int array referring to the triangles within the squares of the mesh
	public int[] triangles;
	//so textures can be added onto the mesh
	public Vector2[] uvs;

	int triangleIndex;

	//set up what each of the parts of the mesh are
	public MeshData(int meshWidth, int meshHeight) {
		vertices = new Vector3[meshWidth * meshHeight];
		uvs = new Vector2[meshWidth * meshHeight];
		triangles = new int[(meshWidth-1)*(meshHeight-1)*6];
	}

	//set up an array for the triangle index
	//every triangle is saved one by one so for example the 2 triangles that make a square will be a and b
	public void AddTriangle(int a, int b, int c) {
		triangles [triangleIndex] = a;
		triangles [triangleIndex + 1] = b;
		triangles [triangleIndex + 2] = c;
		//3 triangles have been made so triangle index is equal to triangle index plus 3
		triangleIndex += 3;
	}

	//actually create the mesh for the object with the set number of triangles and vertices, 
	//values can be changed to change the detail but recommended to keep as they are currently since it's not too many
	//that it crashes or lags excessively, or too few that it looks flat and ugly.
	public Mesh CreateMesh() {
		Mesh mesh = new Mesh ();
		mesh.vertices = vertices;
		mesh.triangles = triangles;
		mesh.uv = uvs;
		//recalculate the normals, use this for lighting so some of the triangles on the mesh do not appear as black
		mesh.RecalculateNormals ();
		return mesh;
	}

}