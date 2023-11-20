using System.Collections;
using System.Collections.Generic;
using Assets.Plugin;
using TMPro;
using UnityEngine;

public class RowingDisplay : MonoBehaviour
{

    [SerializeField, Tooltip("Interface to rowing controller.")]
    private RowingController m_controller;

    private TextMeshProUGUI m_textMeshComponent;

    // Start is called before the first frame update
    void Awake()
    {
        m_textMeshComponent = GetComponent<TextMeshProUGUI>();
    }

    // Update is called once per frame
    void Update()
    {
        RowingData data = m_controller.RowingData;
        string text = $"Connection status: {data.CurrentConnection}";
        if (data.CurrentConnection == RowingData.ConnectionStatus.DataAvailable)
        {
            text +=
                $"\nIsRowing: {data.IsRowing}\nStroke: {data.MovementState}\nDistance: {data.Distance}m\nTime: {data.Time}s\nVelocity: {data.Velocity}m/s";
        }

        m_textMeshComponent.text = text;
    }
}
