using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Xml.Linq;
using Assets.Plugin;
using UnityEngine;


/// <summary>
/// The basic interface to access the data from the concept 2 ergometer.
/// </summary>
public class RowingController : MonoBehaviour
{
    public RowingData RowingData { get; private set; }

    public RowingController()
    {
        RowingData = new RowingData();
    }

    #region Native calls 

    [DllImport("Concept2Plugin", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr GetData();

    [DllImport("Concept2Plugin", CallingConvention = CallingConvention.Cdecl)]
    private static extern void Disconnect();


    #endregion


    // Update is called once per frame
    void Update()
    {
        RowingData = new RowingData(GetData());
    }

    void OnDestroy()
    {
        Disconnect();
    }

}
