window.DTEReleaseGuardQA=(()=>{const exports={};
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.guardCubismRendererRelease = guardCubismRendererRelease;
const installed = new WeakSet();
function guardCubismRendererRelease(value) {
    if (!value || typeof value !== "object")
        return false;
    const renderer = value;
    if (typeof renderer.release !== "function" ||
        !renderer._bufferData ||
        !["vertex", "uv", "index"].every((k) => Object.prototype.hasOwnProperty.call(renderer._bufferData, k)))
        return false;
    if (installed.has(renderer))
        return true;
    const original = renderer.release;
    let released = false;
    renderer.release = () => {
        if (released)
            return;
        if (renderer._clippingManager) {
            original.call(renderer);
        }
        else {
            const data = renderer._bufferData;
            if (!data) {
                original.call(renderer);
                return;
            }
            for (const key of ["vertex", "uv", "index"]) {
                if (data[key] != null)
                    renderer.gl?.deleteBuffer(data[key]);
                data[key] = null;
            }
            renderer._bufferData = undefined;
            renderer._textures = undefined;
        }
        released = true;
    };
    installed.add(renderer);
    return true;
}

return exports;})();
