using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThePlayerViewport : MonoBehaviour 
{
	//vector2 rather than vector3 since only 2 axis are affected by moving the mouse to look
	Vector2 mouseCamLook;
	Vector2 lookSmoothing;
	//floats that make the moving less janky and how fast the camera moves in regards to moving the mouse
	public float sensitivity = 2;
	public float smoothing = 2;
	//a reference to the player object that the camera that this script is attached to
	public GameObject thePlayerChar;

	void Start () 
	{
		//on start get the parent object to the object that the script is attached to
		//so basically just get the player character that the camera that has this script attached
		thePlayerChar = this.transform.parent.gameObject;

		//set the 2 floats just for safety's sake
		sensitivity = 2;
		smoothing = 2;
	}
	
	//on update, not fixed update
	void Update () 
	{
		//create the variable of lookdir for thge look direction. Takes in the mouse movement along the x and y axis
		var lookDir = new Vector2(Input.GetAxisRaw("Mouse X"), Input.GetAxisRaw("Mouse Y"));
		//so then when the mouse is moved,  look direction is multiplied by the sensitivity and smoothing squared
		lookDir = Vector2.Scale(lookDir, new Vector2(sensitivity * smoothing, sensitivity * smoothing));

		//use a lerp, stops the camera from jutting and moving instantly, makes the movement look a lot smoother
		lookSmoothing.x = Mathf.Lerp(lookSmoothing.x, lookDir.x, 1f / smoothing);
		lookSmoothing.y = Mathf.Lerp(lookSmoothing.y, lookDir.y, 1f / smoothing);
		//mousecamlook is equal to mouseCamlook + look smoothing, once again exists just to make it look nicer
		mouseCamLook += lookSmoothing;
		//just to clamp the camera at 90 degrees so you can't look too much up or down and cause issues
		mouseCamLook.y = Mathf.Clamp(mouseCamLook.y, -90f, 90f);

		//this causes the camera to actually rotate around the character fixed on the y axis
		//used the "-" simply for personal preference, rather than inverted.
		transform.localRotation = Quaternion.AngleAxis(-mouseCamLook.y, Vector3.right);
		//and this causes the player to actually turn right an left by moving the mouse accordingly
		thePlayerChar.transform.localRotation = Quaternion.AngleAxis(mouseCamLook.x, thePlayerChar.transform.up);
	}
}
