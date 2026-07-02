#include "message.h"
#include "sound.h"
#include "vars.h"
#include "progress.h"
#include <cmath>
#include <algorithm>

message_t bigMessage;
message_t message;

const char *VariableMsg(const char *txt)
{
	static char out[256];
	int i,outpos,j;
	char num[16];

	outpos=0;
	out[outpos]='\0';
	for(i=0;i<(int)strlen(txt);i++)
	{
		if(txt[i]!='%')
		{
			out[outpos++]=txt[i];
			out[outpos]='\0';
		}
		else
		{
			if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='N' && txt[i+3]=='M')
			{
				for(j=0;j<(int)strlen(profile.name);j++)
				{
					out[outpos++]=profile.name[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='V')
			{
				sprintf(num,"%d",GetVar(VAR_LOCAL+txt[i+3]-'0'));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && txt[i+2]=='G')
			{
				sprintf(num,"%d",GetVar(VAR_GLOBAL+txt[i+3]-'0'));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else if(i<(int)strlen(txt)-3 && txt[i+1]=='%' && GetSpecialVarFunc(txt[i+2]))
			{
				sprintf(num,"%d",GetSpecialVarFunc(txt[i+2])(txt[i+3]));
				for(j=0;j<(int)strlen(num);j++)
				{
					out[outpos++]=num[j];
					if(outpos>=255)
						break;
				}
				i+=3;
			}
			else	// must be a normal %
			{
				out[outpos++]='%';
			}
		}
		out[outpos]='\0';
		if(outpos>=255)
			break;
	}
	return out;
}

void InitMessage(void)
{
	message.msg[0]='\0';
}

void NewBigMessage(const char *txt,int time)
{
	SDL_strlcpy(bigMessage.msg, VariableMsg(txt), sizeof(bigMessage.msg));
	bigMessage.y = GetDisplayMGL()->GetHeight()/2;
	bigMessage.x = -GetStrLength(bigMessage.msg,0)/2;
	bigMessage.dy = 0;
	bigMessage.dx = 13;
	// start big timer
	bigMessage.timer = 0;
	bigMessage.maxTime = time;
	bigMessage.bright = -32;
	bigMessage.brightDir=2;
}

void NewMessage(const char *txt,int time=120,byte priority=0)
{
	if(message.priority==1 && priority==0)
		return;	// can't override it
	SDL_strlcpy(message.msg, VariableMsg(txt), sizeof(message.msg));
	message.x=2;
	message.y=GetDisplayMGL()->GetHeight() + 4;
	message.dy=-13;
	message.timer=0;
	message.maxTime=time;
	message.bright=-32;
	message.brightDir=2;
	message.priority=priority;
}

byte NoRepeatNewMessage(const char *txt,int time,byte priority)
{
	if(message.timer>priority && !strncmp(message.msg,txt,32))
		return 0;	// don't reset if showing the same message
	NewMessage(txt,time,1);
	return 1;
}

float EaseOutCubic(float t)
{
    return 1.0f - powf(1.0f - t, 3.0f);
}

float EaseInCubic(float t)
{
    return t * t * t;
}

void UpdateBigMessage(void)
{
	const float screenW = (float)GetDisplayMGL()->GetWidth();
	const float screenH = (float)GetDisplayMGL()->GetHeight();

	bigMessage.y = screenH * 0.5f;

	bigMessage.timer++;

	// 0-60 frames: slide in
	if (bigMessage.timer < 60)
	{
		float startX = -GetStrLength(bigMessage.msg, 0); // offscreen left
		bigMessage.x = startX + (((screenW * 0.5f) - GetStrLength(bigMessage.msg, 0) * 0.5f) -startX) * EaseOutCubic(bigMessage.timer / 60.0f);
	}
	// 60-180 frames: hold center
	else if (bigMessage.timer < bigMessage.maxTime)
	{
		bigMessage.x = (screenW * 0.5f) - GetStrLength(bigMessage.msg, 0) * 0.5f;
	}
	// 180-240 frames: slide out right
	else if (bigMessage.timer < bigMessage.maxTime + 60.0f)
	{
		float t = (bigMessage.timer - bigMessage.maxTime) / 60.0f;
		t = std::clamp(t, 0.0f, 1.0f);
		float centerX = (screenW * 0.5f) - GetStrLength(bigMessage.msg, 0) * 0.5f;
		float endX = screenW + GetStrLength(bigMessage.msg, 0);

		bigMessage.x = centerX + (endX - centerX) * EaseInCubic(t);
	}
	else
	{
		bigMessage.msg[0] = '\0';
	}
}

void UpdateMessage(void)
{
	UpdateBigMessage();

	message.y+=message.dy;
	message.dy+=1;
	message.bright+=message.brightDir;

	if(message.timer < message.maxTime)
		message.timer++;
	else
		message.brightDir=-2;

	// while time still remains, don't start falling offscreen
	if(message.timer < message.maxTime)
	{
		if(message.dy>0)
			message.dy=0;

		if(message.bright>=32)
			message.brightDir=-2;
		if(message.brightDir<0 && message.bright<0)
		{
			message.brightDir=0;
			message.bright=0;
		}
	}
	else	// go ahead and fall
	{
		if(message.y>GetDisplayMGL()->GetHeight())
		{
			message.msg[0]='\0';
			message.y=0;
			message.dy=0;
			message.priority=0;
		}
	}
}

void RenderMessage(int clock)
{
	char b;

	b=message.bright/2;
	PrintWavy(message.x,message.y,message.msg,b,0,clock+message.timer,1,0.5);
	b=bigMessage.bright/2;
	PrintWavy(bigMessage.x,bigMessage.y,bigMessage.msg,b,0,clock+message.timer,2,1);
}
