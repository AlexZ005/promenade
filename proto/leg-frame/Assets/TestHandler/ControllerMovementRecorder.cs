﻿using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class ControllerMovementRecorder : MonoBehaviour 
{
    /*
     * 
        The objective function to be minimized is defined as: fobj(P) = wdfd +wvfv +whfh +wrfr
        where fd measures the deviation of the motion from available ref- erence data (m), fv measures 
     * the average deviation from the de- sired speed in both sagittal and coronal directions (m/s), 
     * fh mea- sures head accelerations (m/s2), and fr measures whole body ro- tations (degrees). 
     * These terms are weighted usingwd = 100,wv = 5,wh = 0.5,wr = 5. This objective function is used for 
     * all gaits, although the initial parameter values, initial quadruped state, target velocities, 
     * gait graph, and reference data will be different for each specific gait. 
     * 
     * The fr term measures the
     * difference between the desired heading and the actual heading of the character, as measured in 
     * degrees, by the arc cos(x · ˆx), where x and ˆx are the actual and desired forward pointing axes of a leg frame. 
     * This ensures that the quadruped runs forwards rather than sideways. The velocity error term, fv, is 
     * defined as ||v − vd||, and encompasses both sagittal and coronal directions. v is the mean ve- locity as 
     * measured over a stride. The desired velocity in the coronal plane is zero. The desired velocity in the 
     * sagittal plane is an input parameter for the desired gait.

    fobj(P) = +wvfv +(whfh) +wrfr
     * 
     * */

    public Controller m_myController;
    List<double> m_fvVelocityDeviations = new List<double>(); // (current, mean)-desired
    List<Vector3> m_fpMovementDist = new List<Vector3>(); // travel distance
    List<double> m_fhHeadAcceleration = new List<double>();
    List<List<float>> m_frBodyRotationDeviations = new List<List<float>>(); //per-leg frame, arcos(current,desired)
    public float m_fvWeight = 5.0f;
    public float m_fhWeight = 0.5f;
    public float m_frWeight = 5.0f;
    public float m_fpWeight = 0.5f;

    List<Vector3> m_temp_currentStrideVelocities = new List<Vector3>(); // used to calculate mean stride velocity
    List<Vector3> m_temp_currentStrideDesiredVelocities = new List<Vector3>();


	// Use this for initialization
	void Start () 
    {
        int legframes = m_myController.m_legFrames.Length;
        for (int i = 0; i < legframes; i++)
        {
            m_frBodyRotationDeviations.Add(new List<float>());
        }
	}
	
	// Update is called once per frame
	void Update () 
    {
        fv_calcStrideMeanVelocity();
        fr_calcRotationDeviations();
        fh_calcHeadAccelerations();
	}

    void fv_calcStrideMeanVelocity(bool p_forceStore=false)
    {
        GaitPlayer player = m_myController.m_player;
        bool restarted = player.checkHasRestartedStride_AndResetFlag();
        if (!restarted && !p_forceStore)
        {
            m_temp_currentStrideVelocities.Add(m_myController.m_currentVelocity);
            m_temp_currentStrideDesiredVelocities.Add(m_myController.m_desiredVelocity);
        }
        else
        {
            Vector3 totalVelocities=Vector3.zero, totalDesiredVelocities=Vector3.zero;
            for (int i = 0; i < m_temp_currentStrideVelocities.Count; i++)
            {
                totalVelocities += m_temp_currentStrideVelocities[i]; 
                // force straight movement behavior from tests, set desired coronal velocity to constant zero:
                totalDesiredVelocities += new Vector3(0.0f,0.0f,m_temp_currentStrideDesiredVelocities[i].z);
            }
            //Debug.Log("TV: " + totalVelocities + " c " + m_temp_currentStrideVelocities.Count);
            totalVelocities /= (float)m_temp_currentStrideVelocities.Count;
            totalDesiredVelocities /= (float)m_temp_currentStrideDesiredVelocities.Count;
            // add to lists
            //Debug.Log("TV: " + totalVelocities.x + " " + totalVelocities.y + " " + totalVelocities.z);
            m_fvVelocityDeviations.Add((double)Vector3.Magnitude(totalVelocities - totalDesiredVelocities));
            //
            m_temp_currentStrideVelocities.Clear();
            m_temp_currentStrideDesiredVelocities.Clear();
        }
    }

    void fr_calcRotationDeviations()
    {
        int legframes = m_myController.m_legFrames.Length;
        GaitPlayer player = m_myController.m_player;
        for (int i = 0; i < legframes; i++)
        {
            Quaternion currentDesiredOrientation = m_myController.m_legFrames[i].getCurrentDesiredOrientation(player.m_gaitPhase);
            Quaternion currentOrientation = m_myController.m_legFrames[i].transform.rotation;
            Quaternion diff = Quaternion.Inverse(currentOrientation) * currentDesiredOrientation;
            Vector3 axis; float angle;
            diff.ToAngleAxis(out angle,out axis);
            m_frBodyRotationDeviations[i].Add(angle);
        }
    }

    void fh_calcHeadAccelerations()
    {
        m_fhHeadAcceleration.Add((double)m_myController.m_headAcceleration.magnitude);
    }

    void fp_calcMovementDistance()
    {
        m_fpMovementDist.Add(m_myController.transform.position - m_myController.m_oldPos);
    }

    public double Evaluate()
    {
        fv_calcStrideMeanVelocity(true);
        double fv = EvaluateFV();
        double fr = EvaluateFR();
        double fh = EvaluateFH();
        double fp = EvaluateFP();
        double fobj = (double)m_fvWeight*fv + (double)m_frWeight*fr + (double)m_fhWeight*fh + (double)m_fpWeight*fp;
        Debug.Log(fobj+" = "+(double)m_fvWeight*fv+" + "+(double)m_frWeight*fr+" + "+(double)m_fhWeight*fh+" + "+(double)m_fpWeight*fp);
        return fobj;
    }                 
                      
    // Return standard deviation of fv term
    // as small deviations as possible
    public double EvaluateFV()
    {
        double total = 0.0f;
        // mean
        foreach (double d in m_fvVelocityDeviations)
        {
            total += d;
        }
        double avg = total /= (double)(m_fvVelocityDeviations.Count);
        double totmeandiffsqr = 0.0f;
        // std
        foreach (double d in m_fvVelocityDeviations)
        {
            double mdiff = d - avg;
            totmeandiffsqr += mdiff * mdiff;
        }
        double sdeviation = Math.Sqrt(totmeandiffsqr / (double)m_fvVelocityDeviations.Count);
        return avg;
    }

    // mean of FR
    // as small angle difference as possible
    public double EvaluateFR()
    {
        double total = 0.0f;
        int sz = 0;
        // mean
        foreach (List<float> fl in m_frBodyRotationDeviations)
        foreach (float f in fl)
        {
            total += (double)f;
            sz++;
        }
        double avg = total /= (double)(sz);
        return avg;
    }

    // mean of FH
    // as small distance as possible
    public double EvaluateFH()
    {
        double total = 0.0f;
        int sz = 0;
        // mean
        foreach (double d in m_fhHeadAcceleration)
        {
            total += d;
            sz++;
        }
        double avg = total /= (double)(sz);
        return avg;
    }
    
    public double EvaluateFP()
    {
        Vector3 total = Vector3.zero;
        // mean
        foreach (Vector3 dist in m_fpMovementDist)
        {
            total += dist;
        }
        float movementSign = m_myController.m_goalVelocity.z / Mathf.Abs(m_myController.m_goalVelocity.z);
        double scoreInRightDir = Math.Max(0.0,100.0f + total.z * -movementSign); 
        return scoreInRightDir;
    }

}

