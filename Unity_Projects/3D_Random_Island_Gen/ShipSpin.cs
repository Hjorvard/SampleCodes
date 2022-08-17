using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ShipSpin : MonoBehaviour 
{

	//simple script that makes the object spin on the spot.
	void Update () 
	{
	 transform.Rotate (new Vector3 (15, 30, 45) * Time.deltaTime);	
	}
}
