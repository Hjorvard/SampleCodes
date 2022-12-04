using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class scp_GauntletItem_T : MonoBehaviour
{
    public scp_PlayerController_T playerController;

    private void Awake()
    {
        playerController = FindObjectOfType<scp_PlayerController_T>();
    }

    public void PickupGauntlet()
    {
        playerController.GetGauntlet();
        Destroy(this.gameObject);
    }
}
