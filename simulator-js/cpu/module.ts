// Following design pattern:
// https://medium.com/visual-development/how-to-fix-nasty-circular-dependency-issues-once-and-for-all-in-javascript-typescript-a04c987cf0de

import { Logger } from "tslog";
const log: Logger = new Logger({ name: "cpu::module" });
log.silly("cpu::module START");

export import cpu = require("./cpu");

export * from "./registers";
export * from "./if_stage";

export * from "./ram";
export * from "./rom";
export * from "./ppb";

log.silly("cpu::module END");