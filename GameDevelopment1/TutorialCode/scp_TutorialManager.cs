using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class scp_TutorialManager : MonoBehaviour
{
    public GameObject ThePlayer;

    public int Stone;

    private void Awake()
    {
        ThePlayer = GameObject.FindGameObjectWithTag("Player");

        Stone = 0;
    }

    public void IncreaseStone()
    {
        Stone++;
    }

}
