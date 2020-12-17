﻿using System;
using System.ComponentModel;
using Godot;

/// <summary>
///   Definition of a compound in the game. For all other simulation
///   parameters that refer to a compound, there must be an existing
///   entry of this type
/// </summary>
[TypeConverter(typeof(CompoundStringConverter))]
public class Compound : IRegistryType
{
    /// <summary>
    ///   Display name for the user to see
    /// </summary>
    [TranslateFrom("untranslatedName")]
    public string Name;

    public float Volume;

    public bool IsCloud;

    /// <summary>
    ///   When this is true the compound is always considered to be
    ///   useful and is not dumped.
    /// </summary>
    public bool IsAlwaysUseful;

    public bool IsEnvironmental;

    public Color Colour;

#pragma warning disable 169 // Used through reflection
    private string untranslatedName;
#pragma warning restore 169

    public string InternalName { get; set; }

    public void Check(string name)
    {
        if (string.IsNullOrEmpty(Name))
        {
            throw new InvalidRegistryDataException(name, GetType().Name,
                "Compound has no name");
        }

        // Guards against uninitialized alpha
#pragma warning disable RECS0018
        if (Colour.a == 0.0f)
#pragma warning restore RECS0018
            Colour.a = 1;

        if (Math.Abs(Colour.a - 1.0f) > MathUtils.EPSILON)
        {
            throw new InvalidRegistryDataException(name, GetType().Name,
                "Compound colour cannot have alpha other than 1");
        }

        if (Math.Abs(Colour.r) < MathUtils.EPSILON &&
            Math.Abs(Colour.g) < MathUtils.EPSILON && Math.Abs(Colour.b) < MathUtils.EPSILON)
        {
            throw new InvalidRegistryDataException(name, GetType().Name,
                "Compound colour can't be black");
        }

        if (Volume <= 0)
        {
            throw new InvalidRegistryDataException(name, GetType().Name,
                "Volume should be > 0");
        }

        TranslationHelper.CopyTranslateTemplatesToTranslateSource(this);
    }

    public void ApplyTranslations()
    {
        TranslationHelper.ApplyTranslations(this);
    }

    public override string ToString()
    {
        return Name;
    }

    public override int GetHashCode()
    {
        return InternalName?.GetHashCode() ?? 0;
    }

    public override bool Equals(object obj)
    {
        return ReferenceEquals(this, obj) || (obj is Compound other && Equals(other));
    }

    protected bool Equals(Compound other)
    {
        return InternalName == other.InternalName;
    }
}
