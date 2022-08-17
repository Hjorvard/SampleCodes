using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapDisplay : MonoBehaviour 
{
	//the renderer for the texture
	public Renderer textureRenderer;
	//the Meshfilter used for the mesh
	public MeshFilter meshFilter;
	//the renderer used for the mesh
	public MeshRenderer meshRenderer;

	//draw texture function for drawing a 2D texture on the plane
	public void DrawTexture (Texture2D texture)
	{
		//shared material allows it to be seen in the scene view
		textureRenderer.sharedMaterial.mainTexture = texture;
		//renders the image on the plane
		textureRenderer.transform.localScale = new Vector3 (texture.width, 1, texture.height);
	}

	//the DrawMesh function for creating the mesh of the level
	public void DrawMesh (MeshData meshData, Texture2D texture)
	{
		//like shared material allows it to be seen in the scene view
		meshFilter.sharedMesh = meshData.CreateMesh ();
		//render the colourmap on the mesh
		meshRenderer.sharedMaterial.mainTexture = texture;
	}

}