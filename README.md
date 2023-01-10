# Quantum Script Extension Job

```javascript
Job();
this.onStart(process);
this.onEnd(process);
this.onTimedout(process);
this.processMaxCount;
this.processMaxTicks;
this.clockTick;
this.setProcessMaxTime(miliSeconds);
this.setProcessMaxTimeSeconds(seconds);
this.setProcessMaxTimeMinutes(minutes);
this.addProcess(cmd,info,synchronizedKey);
this.addThread(fn,fnThis,parameters,info,synchronizedKey);
this.process();
```

## License

Copyright (c) 2016-2023 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
