using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class Noise
{
	//Returns a 2D array of float values (The comma means that it's a 2D value that is returned)
	public static float[,] GenerateNoiseMap(int mapWidth, int mapHeight, int seed, float scale, int octaves, float persistance, float lacunarity, Vector2 offset)
	{
		float[,] noiseMap  = new float[mapWidth, mapHeight];

		//pseudo random number generator to generate the seed
		System.Random prng = new System.Random (seed);
		Vector2[] octaveOffsets = new Vector2[octaves];
		for (int i = 0; i < octaves; i++)
		{
			//the various offsets that the map can have
			//together these make the variety of seeds the map can have which in this case is 2147483647 (technically it is double this is I
			//use negative values for the seed, but chose against this to be 100% sure it works)
			float offsetX = prng.Next (-150000, 150000) +offset.x;
			float offsetY = prng.Next (-150000, 150000) +offset.y;
			//the amount that the octaves can change the shape of the indivdual seed
			octaveOffsets [i] = new Vector2 (offsetX, offsetY);
		} 

		//An if statement that changes the float "scale" from 0, to a minute number in order to prevent any division by 0 errors
		if (scale <= 0)
		{
			scale = 0.0001f;
		}

		float maxNoiseHeight = float.MinValue;
		float minNoiseHeight = float.MaxValue;

		float halfWidth = mapWidth / 2f;
		float halfHeight = mapHeight / 2f;
		
		for (int y = 0; y < mapHeight; y++) 
		{
			for (int x = 0; x < mapWidth; x++) 
			{

				float amplitude = 1;
				float frequency = 1;
				float noiseHeight = 1;

				for (int i = 0; i < octaves; i++)
				{
					float sampleX = (x - halfWidth) / scale * frequency + octaveOffsets[i].x;
					float sampleY = (y - halfHeight) / scale * frequency + octaveOffsets[i].y;

					float perlinValue = Mathf.PerlinNoise (sampleX, sampleY) * 2 - 1;
					noiseHeight += perlinValue * amplitude;

					amplitude *= persistance;
					frequency *= lacunarity;
				}

				if (noiseHeight > maxNoiseHeight)
				{
					maxNoiseHeight = noiseHeight;
				}
				else if (noiseHeight < minNoiseHeight)
				{
					minNoiseHeight = noiseHeight;
				}

				noiseMap [x, y] = noiseHeight;
			}
		}

		//for loop to cover each block of the map
		for (int y = 0; y < mapHeight; y++)
		{
			for (int x = 0; x < mapWidth; x++)
			{
				//use a lerp in order to make it more than just black and white, allowing for the use of greys
				//that can then be used in generating a colour map with greys being values higher than 0 but smaller than 1
				noiseMap [x, y] = Mathf.InverseLerp (minNoiseHeight, maxNoiseHeight, noiseMap [x, y]);
			}
		}

		return noiseMap;
	}
	
	
}
