#include <gtest/gtest.h>
#include <value.h>

using namespace hfsmexec;
 
TEST(ValueTest, CreateValue)
{
    Value b = true;
    EXPECT_EQ(true, b.getBoolean());

    Value i1 = 42;
    EXPECT_EQ(42, i1.getInteger());

    Value i2 = 42L;
    EXPECT_EQ(42, i2.getInteger());

    Value f1 = 0.42F;
    EXPECT_NEAR(0.42, f1.getFloat(), 0.000001);

    Value f2 = 0.42;
    EXPECT_NEAR(0.42, f2.getFloat(), 0.000001);

    Value s1 = "foobar";
    EXPECT_EQ("foobar", s1.getString());

    Value s2 = std::string("foobar");
    EXPECT_EQ("foobar", s2.getString());

    Value s3 = QString("foobar");
    EXPECT_EQ("foobar", s3.getString());

    Value::Array arr;
    arr.push_back(Value(42));
    Value a = arr;
    EXPECT_EQ(arr, a.getArray());

    Value::Object obj;
    obj["foobar"] = Value(42);
    Value o1 = obj;
    EXPECT_EQ(obj, o1.getObject());

    Value o2 = o1;
    EXPECT_EQ(o1, o2);
}

TEST(ValueTest, SetValue)
{
    Value b;
    b.set(true);
    EXPECT_EQ(true, b.getBoolean());

    Value i1;
    i1.set(42);
    EXPECT_EQ(42, i1.getInteger());

    Value i2;
    i2.set(42L);
    EXPECT_EQ(42, i2.getInteger());

    Value f1;
    f1.set(0.42F);
    EXPECT_NEAR(0.42, f1.getFloat(), 0.000001);

    Value f2;
    f2.set(0.42);
    EXPECT_NEAR(0.42, f2.getFloat(), 0.000001);

    Value s1;
    s1.set("foobar");
    EXPECT_EQ("foobar", s1.getString());

    Value s2;
    s2.set(std::string("foobar"));
    EXPECT_EQ("foobar", s2.getString());

    Value s3;
    s3.set(QString("foobar"));
    EXPECT_EQ("foobar", s3.getString());

    Value::Array arr;
    arr.push_back(Value(42));
    Value a;
    a.set(arr);
    EXPECT_EQ(arr, a.getArray());

    Value::Object obj;
    obj["foobar"] = Value(42);
    Value o1;
    o1.set(obj);
    EXPECT_EQ(obj, o1.getObject());

    Value o2;
    o2.set(o1);
    EXPECT_EQ(o1, o2);
}

TEST(ValueTest, setReference)
{
    Value v1;
    v1["foo"]["bar"] = 42;

    Value v2 = v1;
    v2["foo"] = 42;
    EXPECT_NE(v1, v2);

    Value v3 = &v1;
    v1["foo"]["bar2"] = -42;
    v3["foo"]["bar3"] = 420;
    EXPECT_EQ(v1, v3);
}

TEST(ValueTest, Types)
{
    Value u;
    u.undefined();
    EXPECT_TRUE(u.isUndefined());
    Value n;
    n.null();
    EXPECT_TRUE(n.isNull());
    EXPECT_TRUE(Value(true).isBoolean());
    EXPECT_TRUE(Value(42).isInteger());
    EXPECT_TRUE(Value(42L).isInteger());
    EXPECT_TRUE(Value(0.42F).isFloat());
    EXPECT_TRUE(Value(0.42).isFloat());
    EXPECT_TRUE(Value("foo").isString());
    EXPECT_TRUE(Value(std::string("foo")).isString());
    EXPECT_TRUE(Value(QString("foo")).isString());
    EXPECT_TRUE(Value(Value::Array()).isArray());
    EXPECT_TRUE(Value(Value::Object()).isObject());
}

TEST(ValueTest, ObjectAccess)
{
    Value v1;
    v1["foo"]["bar"] = 42;
    v1["x"]["y"];

    EXPECT_EQ(42, v1["foo"]["bar"].getInteger());
    EXPECT_TRUE(v1["x"]["y"].isNull());

    const Value v2;
    v2["x"]["y"];

    EXPECT_FALSE(v2["x"]["y"].isValid());
}

TEST(ValueTest, ArrayAccess)
{
    Value v1;
    v1[0][0] = 42;
    v1[4][5] = 42;
    v1[1][0];

    EXPECT_EQ(42, v1[0][0].getInteger());
    EXPECT_EQ(42, v1[4][5].getInteger());
    EXPECT_EQ(5, v1.size());
    EXPECT_EQ(6, v1[4].size());
    EXPECT_TRUE(v1[1][0].isNull());

    const Value v2;
    v2[0][0];
    v2[4][5];

    EXPECT_FALSE(v2[0][0].isValid());
    EXPECT_FALSE(v2[4][5].isValid());
}

TEST(ValueTest, unite)
{
    Value v1;
    v1["a"]["b"] = 42;
    v1["a"]["c"] = -420;

    Value v2;
    v2["a"]["d"] = -42;
    v2["a"]["c"] = 420;

    v1.unite(v2);

    EXPECT_EQ(42, v1["a"]["b"].getInteger());
    EXPECT_EQ(420, v1["a"]["c"].getInteger());
    EXPECT_EQ(-42, v1["a"]["d"].getInteger());

    Value v3;
    v3["a"][0] = 42;
    v3["a"][2] = -420;
    v3["a"][3] = 42;

    Value v4;
    v4["a"][1] = -42;
    v4["a"][2] = 420;

    v3.unite(v4);

    EXPECT_TRUE(v3["a"][0].isNull());
    EXPECT_EQ(-42, v3["a"][1].getInteger());
    EXPECT_EQ(420, v3["a"][2].getInteger());
    EXPECT_EQ(42, v3["a"][3].getInteger());
}

void verifyValueStructure(Value& value)
{
    EXPECT_TRUE(value.isObject());
    EXPECT_TRUE(value["v1"].getBoolean());
    EXPECT_EQ(42, value["v2"].getInteger());
    EXPECT_NEAR(0.42, value["v3"].getFloat(), 0.000001);
    EXPECT_EQ("foobar", value["v4"].getString());
    EXPECT_TRUE(value["v5"].isArray());

        EXPECT_FALSE(value["v5"][0].getBoolean());
        EXPECT_EQ(420, value["v5"][1].getInteger());
        EXPECT_NEAR(-0.42, value["v5"][2].getFloat(), 0.000001);
        EXPECT_EQ("bar", value["v5"][3].getString());
        EXPECT_TRUE(value["v5"][4].isArray());
        EXPECT_TRUE(value["v5"][4][0].isObject());

            EXPECT_EQ("foo", value["v5"][4][0]["v1"].getString());
            EXPECT_NEAR(4.2, value["v5"][4][0]["v2"].getFloat(), 0.000001);

            EXPECT_EQ("bar", value["v5"][4][1]["v1"].getString());
            EXPECT_NEAR(0.042, value["v5"][4][1]["v2"].getFloat(), 0.000001);

        EXPECT_EQ("foobar", value["v5"][5]["v1"].getString());
        EXPECT_EQ(420, value["v5"][5]["v2"].getInteger());
}

TEST(ValueTest, XmlSerialization)
{
    QString xmlIn = "<?xml version=\"1.0\"?><value type=\"Object\"><value name=\"v1\" type=\"Boolean\">true</value><value name=\"v2\" type=\"Integer\">42</value><value name=\"v3\" type=\"Float\">0.42</value><value name=\"v4\" type=\"String\">foobar</value><value name=\"v5\" type=\"Array\"><value type=\"Boolean\">false</value><value type=\"Integer\">420</value><value type=\"Float\">-0.42</value><value type=\"String\">bar</value><value type=\"Array\"><value type=\"Object\"><value name=\"v1\" type=\"String\">foo</value><value name=\"v2\" type=\"Float\">4.2</value></value><value type=\"Object\"><value name=\"v1\" type=\"String\">bar</value><value name=\"v2\" type=\"Float\">0.042</value></value></value><value type=\"Object\"><value name=\"v1\" type=\"String\">foobar</value><value name=\"v2\" type=\"Integer\">420</value></value></value></value>";
    Value value;
    value.fromXml(xmlIn);

    QString xmlOut;
    value.toXml(xmlOut);

    verifyValueStructure(value);

    EXPECT_STREQ(xmlIn.toStdString().c_str(), xmlOut.toStdString().c_str());
}

TEST(ValueTest, JsonSerialization)
{
    QString jsonIn = "{\"v1\":true,\"v2\":42,\"v3\":0.42,\"v4\":\"foobar\",\"v5\":[false,420,-0.42,\"bar\",[{\"v1\":\"foo\",\"v2\":4.2},{\"v1\":\"bar\",\"v2\":0.042}],{\"v1\":\"foobar\",\"v2\":420}]}";
    Value value;
    value.fromJson(jsonIn);

    verifyValueStructure(value);

    QString jsonOut;
    value.toJson(jsonOut);

    //EXPECT_STREQ(jsonIn.toStdString().c_str(), jsonOut.toStdString().c_str()); //TODO
}

TEST(ValueTest, YamlSerialization)
{
    QString yamlIn = "v1: true\nv2: 42\nv3: 0.42\nv4: foobar\nv5:\n  - false\n  - 420\n  - -0.42\n  - bar\n  -\n    - v1: foo\n      v2: 4.2\n    - v1: bar\n      v2: 0.042\n  - v1: foobar\n    v2: 420";
    Value value;
    value.fromYaml(yamlIn);

    verifyValueStructure(value);

    QString yamlOut;
    value.toYaml(yamlOut);

    //EXPECT_STREQ(yamlIn.toStdString().c_str(), yamlOut.toStdString().c_str()); //TODO
}
